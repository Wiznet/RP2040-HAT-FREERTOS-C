/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Include library header files ----------------------------------------------*/
/* Include peripheral header files -------------------------------------------*/
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

/* Include On-board hardware library header files ----------------------------*/
#include "rp_eth_io_pins.h"
#include "wizchip_conf.h"
#include "w5x00_spi.h"

/* Include user header files -------------------------------------------------*/
#include "rp_eth_io_init.h"
#include "rp_eth_io_common.h"
#include "rp_eth_io_i2c.h"

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Imported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions prototypes----------------------------------------------*/
void rp_eth_io_init(void);


/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void rp_eth_io_init(void)
{
    gpio_init(STATUS_LED_PIN);
    gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);
    gpio_put(STATUS_LED_PIN, 0);

    gpio_init(IO_EN_PIN);
    gpio_set_dir(IO_EN_PIN, GPIO_OUT);
    gpio_put(IO_EN_PIN, 0);

    gpio_init(RS485_CTRL_PIN);
    gpio_set_dir(RS485_CTRL_PIN, GPIO_OUT);
    gpio_put(RS485_CTRL_PIN, 0);
    uart_init(RP_ETH_IO_UART_PORT, DEFAULT_UART_BAUD_RATE);
    gpio_set_function(RS485_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RS485_RX_PIN, GPIO_FUNC_UART);
    uart_set_hw_flow(RP_ETH_IO_UART_PORT, false, false);

    for(int ch = 0; ch < 4; ch++) {
        gpio_init(npn_input_pins[ch]);
        gpio_set_dir(npn_input_pins[ch], GPIO_IN);
    }

    for(int ch = 0; ch < 4; ch++) {
        gpio_init(npn_output_pins[ch]);
        gpio_set_dir(npn_output_pins[ch], GPIO_OUT);
        gpio_put(npn_output_pins[ch], 0);
    }

    i2c_init(RP_ETH_IO_I2C_PORT, DEFAULT_I2C_BAUD_RATE);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_disable_pulls(SDA_PIN);
    gpio_disable_pulls(SCL_PIN);

    gpio_put(IO_EN_PIN, 1);

    sleep_ms(10);
    uint8_t *mac_addr = get_mac_address(MAC_24AA02Exx_DEV_ADDR);
    for(int i = 0; i < 6; i++) {
        g_net_info.mac[i] = *(mac_addr + i);
    }

    // init Ethernet IC
    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
}


/* Private functions ---------------------------------------------------------*/

/***************************************************************END OF FILE****/
