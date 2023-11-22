/******************************************************************************/
/**
 * @file
 * @brief       main file
 *
 * @author      Y2Kb
 */
/******************************************************************************/

/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Include library header files ----------------------------------------------*/
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "port_common.h"

/* Include peripheral header files -------------------------------------------*/
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include "timer.h"

/* Include On-board hardware library header files ----------------------------*/
#include "rp_eth_io_pins.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"

/* Include user header files -------------------------------------------------*/
#include "dhcp.h"
#include "dns.h"

#include "rp_eth_io_common.h"
#include "rp_eth_io_init.h"
#include "tests.h"
#include "rp_eth_io_adc.h"


/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
// Task
#define DHCP_TASK_STACK_SIZE 2048
#define DHCP_TASK_PRIORITY 8

#define DNS_TASK_STACK_SIZE 512
#define DNS_TASK_PRIORITY 10

// Clock
#define PLL_SYS_KHZ (133 * 1000)

// Buffer
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

// Socket
#define SOCKET_DHCP 0
#define SOCKET_DNS 3

// Retry count
#define DHCP_RETRY_COUNT 5
#define DNS_RETRY_COUNT 5


/* Exported variables --------------------------------------------------------*/
/* Imported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// Network
static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};

// DHCP
static uint8_t g_dhcp_get_ip_flag = 0;

// DNS
static uint8_t g_dns_target_domain[] = "www.wiznet.io";
static uint8_t g_dns_target_ip[4] = {
    0,
};
static uint8_t g_dns_get_ip_flag = 0;

// Semaphore
static xSemaphoreHandle dns_sem = NULL;

// Timer
static volatile uint32_t g_msec_cnt = 0;


/* Exported functions prototypes----------------------------------------------*/
// Task
void dhcp_task(void *argument);
void dns_task(void *argument);


/* Private function prototypes -----------------------------------------------*/
// Clock
static void set_clock_khz(void);

// DHCP
static void wizchip_dhcp_init(void);
static void wizchip_dhcp_assign(void);
static void wizchip_dhcp_conflict(void);

// Timer
static void repeating_timer_callback(void);


/* Exported functions --------------------------------------------------------*/
int main()
{
    /* Initialize */
    set_clock_khz();

    stdio_init_all();

    // Initialize
    rp_eth_io_init();

    sleep_ms(3000);     // Wait for the serial console software (ex. Tera Term) to automatically connect to the RP2040
    printf("start\n");
    printf("RP2040 chip version: %d, ROM verison: %d\n", rp2040_chip_version(), rp2040_rom_version());
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        *(rp_eth_io_mac_addr + 0), *(rp_eth_io_mac_addr + 1), *(rp_eth_io_mac_addr + 2), *(rp_eth_io_mac_addr + 3), *(rp_eth_io_mac_addr + 4), *(rp_eth_io_mac_addr + 5));

    // ----- Peripehral Tests -----
    npn_output_test();
    // npn_input_test();
    // io_test();
    // adc_test();
    // rs485_test();
    // i2c_test();
    // udp_test();

    wizchip_initialize();   // need to connect LAN cable
    wizchip_check();

    wizchip_1ms_timer_initialize(repeating_timer_callback);

    xTaskCreate(dhcp_task, "DHCP_Task", DHCP_TASK_STACK_SIZE, NULL, DHCP_TASK_PRIORITY, NULL);
    xTaskCreate(dns_task, "DNS_Task", DNS_TASK_STACK_SIZE, NULL, DNS_TASK_PRIORITY, NULL);

    dns_sem = xSemaphoreCreateCounting((unsigned portBASE_TYPE)0x7fffffff, (unsigned portBASE_TYPE)0);

    gpio_put(STATUS_LED_PIN, 1);

    vTaskStartScheduler();

    while (1) {
    }
}


// Task
void dhcp_task(void *argument)
{
    int retval = 0;
    uint8_t link;
    uint16_t len = 0;
    uint32_t dhcp_retry = 0;

    if (g_net_info.dhcp == NETINFO_DHCP) // DHCP
    {
        wizchip_dhcp_init();
    }
    else // static
    {
        network_initialize(g_net_info);

        /* Get network information */
        print_network_information(g_net_info);

        while (1)
        {
            vTaskDelay(1000 * 1000);
        }
    }

    while (1)
    {
        link = wizphy_getphylink();

        if (link == PHY_LINK_OFF)
        {
            printf("PHY_LINK_OFF\n");

            DHCP_stop();

            while (1)
            {
                link = wizphy_getphylink();

                if (link == PHY_LINK_ON)
                {
                    wizchip_dhcp_init();

                    dhcp_retry = 0;

                    break;
                }

                vTaskDelay(1000);
            }
        }

        retval = DHCP_run();

        if (retval == DHCP_IP_LEASED)
        {
            if (g_dhcp_get_ip_flag == 0)
            {
                dhcp_retry = 0;

                printf(" DHCP success\n");

                g_dhcp_get_ip_flag = 1;

                xSemaphoreGive(dns_sem);
            }
        }
        else if (retval == DHCP_FAILED)
        {
            g_dhcp_get_ip_flag = 0;
            dhcp_retry++;

            if (dhcp_retry <= DHCP_RETRY_COUNT)
            {
                printf(" DHCP timeout occurred and retry %d\n", dhcp_retry);
            }
        }

        if (dhcp_retry > DHCP_RETRY_COUNT)
        {
            printf(" DHCP failed\n");

            DHCP_stop();

            while (1)
            {
                vTaskDelay(1000 * 1000);
            }
        }

        vTaskDelay(10);
    }
}

void dns_task(void *argument)
{
    uint8_t dns_retry;

    while (1)
    {
        xSemaphoreTake(dns_sem, portMAX_DELAY);
        DNS_init(SOCKET_DNS, g_ethernet_buf);

        dns_retry = 0;

        while (1)
        {
            if (DNS_run(g_net_info.dns, g_dns_target_domain, g_dns_target_ip) > 0)
            {
                printf(" DNS success\n");
                printf(" Target domain : %s\n", g_dns_target_domain);
                printf(" IP of target domain : %d.%d.%d.%d\n", g_dns_target_ip[0], g_dns_target_ip[1], g_dns_target_ip[2], g_dns_target_ip[3]);

                break;
            }
            else
            {
                dns_retry++;

                if (dns_retry <= DNS_RETRY_COUNT)
                {
                    printf(" DNS timeout occurred and retry %d\n", dns_retry);
                }
            }

            if (dns_retry > DNS_RETRY_COUNT)
            {
                printf(" DNS failed\n");

                break;
            }

            vTaskDelay(10);
        }
    }
}


/* Private functions ---------------------------------------------------------*/
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

/* DHCP */
static void wizchip_dhcp_init(void)
{
    printf(" DHCP client running\n");

    DHCP_init(SOCKET_DHCP, g_ethernet_buf);

    reg_dhcp_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);

    g_dhcp_get_ip_flag = 0;
}

static void wizchip_dhcp_assign(void)
{
    getIPfromDHCP(g_net_info.ip);
    getGWfromDHCP(g_net_info.gw);
    getSNfromDHCP(g_net_info.sn);
    getDNSfromDHCP(g_net_info.dns);

    g_net_info.dhcp = NETINFO_DHCP;

    /* Network initialize */
    network_initialize(g_net_info); // apply from DHCP

    print_network_information(g_net_info);
    printf(" DHCP leased time : %ld seconds\n", getDHCPLeasetime());
}

static void wizchip_dhcp_conflict(void)
{
    printf(" Conflict IP from DHCP\n");

    // halt or reset or any...
    while (1)
    {
        vTaskDelay(1000 * 1000);
    }
}

/* Timer */
static void repeating_timer_callback(void)
{
    g_msec_cnt++;

    if (g_msec_cnt >= 1000 - 1)
    {
        g_msec_cnt = 0;

        DHCP_time_handler();
        DNS_time_handler();
    }
}
