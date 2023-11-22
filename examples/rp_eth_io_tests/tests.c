/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Include library header files ----------------------------------------------*/
/* Include peripheral header files -------------------------------------------*/
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

/* Include On-board hardware library header files ----------------------------*/
#include "rp_eth_io_pins.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"
#include "socket.h"

/* Include user header files -------------------------------------------------*/
#include "tests.h"
#include "rp_eth_io_common.h"
#include "rp_eth_io_adc.h"
#include "rp_eth_io_i2c.h"


/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Imported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions prototypes----------------------------------------------*/
void npn_input_test(void);
void npn_output_test(void);
void io_test(void);
void adc_test(void);
void rs485_test(void);
void i2c_test(void);
void udp_test(void);


/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void npn_input_test(void)
{
    for(;;) {
        bool states[4];
        int num = 0;
        for(int i = 0; i < 4; i++) {
            states[i] = gpio_get(npn_input_pins[i]);
            num += states[i] ? 1 : 0;
        }
        printf("%d,%d,%d,%d\n", states[0]?1:0, states[1]?1:0, states[2]?1:0, states[3]?1:0);
        sleep_ms(1000);
    }
}


void npn_output_test(void)
{
    for(;;) {
        for(int ch = 0; ch < 4; ch++) {
            // Toggle
            gpio_put(npn_output_pins[ch], gpio_get_out_level(npn_output_pins[ch]) ? 0 : 1);
            sleep_ms(300);
        }
    }
}


void io_test(void)
{
    for(int ch = 0; ch < 4; ch++) {
        gpio_init(io_pins[ch]);
        gpio_set_dir(io_pins[ch], GPIO_OUT);
        gpio_put(io_pins[ch], 0);
    }

    for(;;) {
        for(int ch = 0; ch < 4; ch++) {
            // Toggle
            gpio_put(io_pins[ch], gpio_get_out_level(io_pins[ch]) ? 0 : 1);
            sleep_ms(300);
        }
    }
}


void adc_test(void)
{
    for(;;) {
        #define ADC_NUM     1000
        float voltages[4][ADC_NUM];
        float ave_voltages[4] = {0.0f};
        float std_voltages[4] = {0.0f};
        for(int ch = 0; ch < 4; ch++) {
            rp_eth_io_getADCvalue(ch, 10000.0f, ADC_NUM, voltages[ch]);
            for(int i = 0; i < ADC_NUM; i++) {
                ave_voltages[ch] += voltages[ch][i];
            }
            ave_voltages[ch] /= ADC_NUM;

            for(int i = 0; i < ADC_NUM; i++) {
                float buf = voltages[ch][i] - ave_voltages[ch];
                std_voltages[ch] += buf * buf;
            }
            std_voltages[ch] = sqrtf(std_voltages[ch] / ADC_NUM);
        }

        printf("%.4f(%.4f)\t%.4f(%.4f)\t%.4f(%.4f)\t%.4f(%.4f)\n", 
            ave_voltages[0], std_voltages[0],
            ave_voltages[1], std_voltages[1],
            ave_voltages[2], std_voltages[2],
            ave_voltages[3], std_voltages[3]
            );
        sleep_ms(500);
    }
}


// Sensor used for RS-485 test: XY-MD02 (Temperature and Humidity Transmitter)
void rs485_test(void)
{
    uart_set_baudrate(RP_ETH_IO_UART_PORT, 9600);
    uart_set_format(RP_ETH_IO_UART_PORT, 8, 1, UART_PARITY_NONE);

    for(;;) {
        // Clear rx fifo
        while (uart_is_readable(RP_ETH_IO_UART_PORT))
            uart_getc(RP_ETH_IO_UART_PORT);

        gpio_put(RS485_CTRL_PIN, 1);
        uart_puts(RP_ETH_IO_UART_PORT, "READ");
        uart_tx_wait_blocking(RP_ETH_IO_UART_PORT);
        gpio_put(RS485_CTRL_PIN, 0);

        int rx_cnt = 0;
        char rx_str[100] = {'\0'};
        for(;;) {
            bool rx_completed = false;
            while (uart_is_readable(RP_ETH_IO_UART_PORT)) {
                uint8_t c = uart_getc(RP_ETH_IO_UART_PORT);
                rx_str[rx_cnt++] = c;
                if(c == '\r' || c == '\n') {
                    rx_completed = true;
                    break;
                }
            }
            if(rx_completed) break;
        }
        printf("%s\n", rx_str);

        sleep_ms(1000);
    }
}


void i2c_test(void)
{
    for(;;) {
        rp_eth_io_i2c_dev_scan();

        // uint8_t *mac_addr = get_mac_address(MAC_24AA02Exx_DEV_ADDR);
        // printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
        //     *(mac_addr + 0), *(mac_addr + 1), *(mac_addr + 2), *(mac_addr + 3), *(mac_addr + 4), *(mac_addr + 5));

        sleep_ms(5000);
    }
}


void udp_test(void)
{
    wizchip_initialize();   // need to connect LAN cable
    wizchip_check();

    const uint8_t ip[4] = {192, 168, 0, 100};
    const uint8_t gw[4] = {192, 168, 0, 1};
    memcpy(g_net_info.ip, ip, 4);
    memcpy(g_net_info.gw, gw, 4);
    g_net_info.dhcp = NETINFO_STATIC;
    network_initialize(g_net_info);
    print_network_information(g_net_info);

    // Default PHY setting : by=PHY_CONFBY_HW, mode=PHY_MODE_AUTONEGO, speed=PHY_SPEED_10, duplex:PHY_DUPLEX_HALF
    ctlwizchip(CW_SET_PHYCONF, (void*)&g_phyconf);
    ctlwizchip(CW_GET_PHYCONF, (void*)&g_phyconf);
    printf("PHY configuration:\n");
    if(g_phyconf.by    == PHY_CONFBY_HW)   printf("   by     : PHY_CONFBY_HW\n");   else printf("   by     : PHY_CONFBY_SW\n");
    if(g_phyconf.mode  == PHY_MODE_MANUAL) printf("   mode   : PHY_MODE_MANUAL\n"); else printf("   mode   : PHY_MODE_AUTONEGO\n");
    if(g_phyconf.speed == PHY_SPEED_10)    printf("   speed  : PHY_SPEED_10\n");    else printf("   speed  : PHY_SPEED_100\n");
    if(g_phyconf.duplex == PHY_DUPLEX_HALF)printf("   duplex : PHY_DUPLEX_HALF\n"); else printf("   duplex : PHY_DUPLEX_FULL\n");

    // Default timeout setting : retry_cnt=8, time_100us=2000
    g_netTimeout.retry_cnt = 2;
    g_netTimeout.time_100us = 2500;
    ctlnetwork(CN_SET_TIMEOUT, (void*)&g_netTimeout);
    ctlnetwork(CN_GET_TIMEOUT, (void*)&g_netTimeout);
    printf("\nset timeout setting:\n   retry_cnt=%d\n   time_100us=%d\n\n", g_netTimeout.retry_cnt, g_netTimeout.time_100us);


    uint8_t send_data_udp[50] = {'\0'};
    uint8_t rcv_data_udp[50]  = {'\0'};
    const uint16_t myPortNum = 50000, dstPortNum = 4000;
	const uint8_t udp_dstIp[4] = {192, 168, 0, 2};
	const uint8_t sn = 1;

    socket(sn, Sn_MR_UDP, myPortNum, SF_IO_NONBLOCK);
    printf("open socket\r\n");

    for(;;) {
        static int i = 0;
		printf("%d\n", i);

		memset(rcv_data_udp, '\0', sizeof(rcv_data_udp));
		uint16_t rcvPortNum;
		int32_t recvlen = recvfrom(sn, rcv_data_udp, 100, udp_dstIp, &rcvPortNum);
		if(recvlen > 0)
			printf("port:%d, %s, recvlen:%d\r\n", rcvPortNum, rcv_data_udp, (int)recvlen);
		else {
			switch(recvlen)
			{
			case 0:
				// No data
				break;
			case SOCKERR_DATALEN:
				printf("recvfrom error : Data length error\n");
				break;
			case SOCKERR_SOCKMODE:
				printf("recvfrom error : Invalid operation in the socket\n");
				break;
			case SOCKERR_SOCKNUM:
				printf("recvfrom error : Invalid socket number\n");
				break;
			default:
				printf("Unknown recvfrom error\n");
				break;
			}
		}

		sprintf(send_data_udp, "W5500:%d\n", i);
		sendto(sn, send_data_udp, strlen(send_data_udp), udp_dstIp, dstPortNum);

		i++;
        sleep_ms(1000);
    }
}


/* Private functions ---------------------------------------------------------*/

/***************************************************************END OF FILE****/
