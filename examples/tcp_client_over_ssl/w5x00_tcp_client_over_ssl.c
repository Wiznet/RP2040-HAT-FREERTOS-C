/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "socket.h"
#include "w5x00_spi.h"
#include "w5x00_gpio_irq.h"

#include "timer.h"

#include "mbedtls/x509_crt.h"
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/compat-1.3.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Task */
#define TCP_TASK_STACK_SIZE 4096
#define TCP_TASK_PRIORITY 7

#define RECV_TASK_STACK_SIZE 256
#define RECV_TASK_PRIORITY 6

/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* Socket */
#define SOCKET_SSL 2

/* Port */
#define PORT_SSL 443

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 11, 2},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 11, 1},                     // Gateway
        .dns = {8, 8, 8, 8},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

/* SSL */
static uint8_t g_send_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_recv_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_ssl_target_ip[4] = {192, 168, 11, 3};

static mbedtls_ctr_drbg_context g_ctr_drbg;
static mbedtls_ssl_config g_conf;
static mbedtls_ssl_context g_ssl;

/* Semaphore */
static xSemaphoreHandle recv_sem = NULL;

/* Timer  */
static volatile uint32_t g_msec_cnt = 0;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Task */
void tcp_task(void *argument);
void recv_task(void *argument);

void *pvPortCalloc(size_t sNb, size_t sSize);
void pvPortFree(void *vPtr);

/* Clock */
static void set_clock_khz(void);

/* SSL */
static int wizchip_ssl_init(uint8_t *socket_fd);
static int ssl_random_callback(void *p_rng, unsigned char *output, size_t output_len);

/* GPIO  */
static void gpio_callback(void);

/* Timer  */
static void repeating_timer_callback(void);
static time_t millis(void);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
    /* Initialize */
    set_clock_khz();

    stdio_init_all();

    mbedtls_platform_set_calloc_free(pvPortCalloc, pvPortFree);

    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    wizchip_gpio_interrupt_initialize(SOCKET_SSL, gpio_callback);
    wizchip_1ms_timer_initialize(repeating_timer_callback);

    xTaskCreate(tcp_task, "TCP_Task", TCP_TASK_STACK_SIZE, NULL, TCP_TASK_PRIORITY, NULL);
    xTaskCreate(recv_task, "RECV_Task", RECV_TASK_STACK_SIZE, NULL, RECV_TASK_PRIORITY, NULL);

    recv_sem = xSemaphoreCreateCounting((unsigned portBASE_TYPE)0x7fffffff, (unsigned portBASE_TYPE)0);

    vTaskStartScheduler();

    while (1)
    {
        ;
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Task */
void tcp_task(void *argument)
{
    const int *list = NULL;
    int retval = 0;
    uint16_t len = 0;
    uint32_t start_ms = 0;
    uint32_t send_cnt = 0;

    network_initialize(g_net_info);

    /* Get network information */
    print_network_information(g_net_info);

    retval = wizchip_ssl_init(SOCKET_SSL);

    if (retval < 0)
    {
        printf(" SSL initialize failed %d\n", retval);

        while (1)
        {
            vTaskDelay(1000 * 1000);
        }
    }

    /* Get ciphersuite information */
    printf(" Supported ciphersuite lists\n");

    list = mbedtls_ssl_list_ciphersuites();

    while (*list)
    {
        printf(" %-42s\n", mbedtls_ssl_get_ciphersuite_name(*list));

        list++;

        if (!*list)
        {
            break;
        }

        printf(" %s\n", mbedtls_ssl_get_ciphersuite_name(*list));

        list++;
    }

    retval = socket((uint8_t)(g_ssl.p_bio), Sn_MR_TCP, PORT_SSL, SF_TCP_NODELAY);

    if (retval != SOCKET_SSL)
    {
        printf(" Socket failed %d\n", retval);

        while (1)
        {
            vTaskDelay(1000 * 1000);
        }
    }

    start_ms = millis();

    do
    {
        retval = connect((uint8_t)(g_ssl.p_bio), g_ssl_target_ip, PORT_SSL);

        if ((retval == SOCK_OK) || (retval == SOCKERR_TIMEOUT))
        {
            break;
        }

        vTaskDelay(1000);

    } while ((millis() - start_ms) < RECV_TIMEOUT);

    if ((retval != SOCK_OK) || (retval == SOCK_BUSY))
    {
        printf(" Connect failed %d\n", retval);

        while (1)
        {
            vTaskDelay(1000 * 1000);
        }
    }

    printf(" Connected %d\n", retval);

    while ((retval = mbedtls_ssl_handshake(&g_ssl)) != 0)
    {
        if ((retval != MBEDTLS_ERR_SSL_WANT_READ) && (retval != MBEDTLS_ERR_SSL_WANT_WRITE))
        {
            printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n", -retval);

            while (1)
            {
                vTaskDelay(1000 * 1000);
            }
        }
    }

    printf(" ok\n    [ Ciphersuite is %s ]\n", mbedtls_ssl_get_ciphersuite(&g_ssl));

    while (1)
    {
        memset(g_send_buf, 0, ETHERNET_BUF_MAX_SIZE);
        sprintf(g_send_buf, " Send data : %d\n", send_cnt);

        retval = mbedtls_ssl_write(&g_ssl, g_send_buf, strlen(g_send_buf));

        if (retval < 0)
        {
            printf(" failed\n  ! mbedtls_ssl_write returned -0x%x\n", -retval);

            while (1)
            {
                vTaskDelay(1000 * 1000);
            }
        }

        printf(" Send OK : %d\n", send_cnt++);

        vTaskDelay(1000 * 3);
    }
}

void recv_task(void *argument)
{
    uint8_t socket_num;
    uint16_t reg_val;
    uint16_t recv_len;

    while (1)
    {
        xSemaphoreTake(recv_sem, portMAX_DELAY);
        ctlwizchip(CW_GET_INTERRUPT, (void *)&reg_val);
#if _WIZCHIP_ == W5100S
        reg_val &= 0x00FF;
#elif _WIZCHIP_ == W5500
        reg_val = (reg_val >> 8) & 0x00FF;
#endif

        for (socket_num = 0; socket_num < _WIZCHIP_SOCK_NUM_; socket_num++)
        {
            if (reg_val & (1 << socket_num))
            {
                break;
            }
        }

        if (socket_num == SOCKET_SSL)
        {
            reg_val = (SIK_CONNECTED | SIK_DISCONNECTED | SIK_RECEIVED | SIK_TIMEOUT) & 0x00FF; // except SIK_SENT(send OK) interrupt

            ctlsocket(socket_num, CS_CLR_INTERRUPT, (void *)&reg_val);
            getsockopt(socket_num, SO_RECVBUF, (void *)&recv_len);

            if (recv_len > 0)
            {
                memset(g_recv_buf, 0, ETHERNET_BUF_MAX_SIZE);

                mbedtls_ssl_read(&g_ssl, g_recv_buf, (uint16_t)recv_len);

                printf("%s\n", g_recv_buf);
            }
        }
    }
}

void *pvPortCalloc(size_t sNb, size_t sSize)
{
    void *vPtr = NULL;

    if (sSize > 0)
    {
        vPtr = pvPortMalloc(sSize * sNb); // Call FreeRTOS or other standard API

        if (vPtr)
        {
            memset(vPtr, 0, (sSize * sNb)); // Must required
        }
    }

    return vPtr;
}

void pvPortFree(void *vPtr)
{
    if (vPtr)
    {
        vPortFree(vPtr); // Call FreeRTOS or other standard API
    }
}

/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}

/* SSL */
static int wizchip_ssl_init(uint8_t *socket_fd)
{
    int retval;

    mbedtls_ctr_drbg_init(&g_ctr_drbg);
    mbedtls_ssl_init(&g_ssl);
    mbedtls_ssl_config_init(&g_conf);

    if ((retval = mbedtls_ssl_config_defaults(&g_conf,
                                              MBEDTLS_SSL_IS_CLIENT,
                                              MBEDTLS_SSL_TRANSPORT_STREAM,
                                              MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n", retval);

        return -1;
    }

    printf(" Socket descriptor %d\n", socket_fd);

    mbedtls_ssl_conf_authmode(&g_conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&g_conf, ssl_random_callback, &g_ctr_drbg);
    mbedtls_ssl_conf_endpoint(&g_conf, MBEDTLS_SSL_IS_CLIENT);
    mbedtls_ssl_conf_read_timeout(&g_conf, 1000 * 10);

    if ((retval = mbedtls_ssl_setup(&g_ssl, &g_conf)) != 0)
    {
        printf(" failed\n  ! mbedtls_ssl_setup returned %d\n", retval);

        return -1;
    }

    mbedtls_ssl_set_bio(&g_ssl, socket_fd, send, recv, NULL);

    return 0;
}

static int ssl_random_callback(void *p_rng, unsigned char *output, size_t output_len)
{
    int i;

    if (output_len <= 0)
    {
        return 1;
    }

    for (i = 0; i < output_len; i++)
    {
        *output++ = rand() % 0xff;
    }

    srand(rand());

    return 0;
}

/* GPIO */
static void gpio_callback(void)
{
    signed portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(recv_sem, &xHigherPriorityTaskWoken);
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

/* Timer */
static void repeating_timer_callback(void)
{
    g_msec_cnt++;
}

static time_t millis(void)
{
    return g_msec_cnt;
}
