/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Include library header files ----------------------------------------------*/
/* Include peripheral header files -------------------------------------------*/
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

/* Include On-board hardware library header files ----------------------------*/
#include "rp_eth_io_pins.h"

/* Include user header files -------------------------------------------------*/
#include "rp_eth_io_i2c.h"
#include "rp_eth_io_common.h"


/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define I2C_READ_BIT 0x80

/* Exported variables --------------------------------------------------------*/
/* Imported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions prototypes----------------------------------------------*/
void rp_eth_io_i2c_dev_scan(void);
bool reserved_addr(uint8_t);
uint8_t *get_mac_address(uint8_t);
void i2c_read_registers(uint8_t, uint8_t, uint8_t *, uint16_t);


/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void rp_eth_io_i2c_dev_scan(void)
{
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (uint8_t addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(RP_ETH_IO_I2C_PORT, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
}


// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}


uint8_t *get_mac_address(uint8_t dev_addr)
{
    i2c_read_registers(dev_addr, MAC_24AA02Exx_REG_ADDR_MAC + 2, rp_eth_io_mac_addr, 6);
    return rp_eth_io_mac_addr;
}


void i2c_read_registers(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    reg = reg | I2C_READ_BIT;
    i2c_write_blocking(RP_ETH_IO_I2C_PORT, dev_addr, &reg, 1, true); 
    i2c_read_blocking(RP_ETH_IO_I2C_PORT, dev_addr, buf, len, false);
}


/* Private functions ---------------------------------------------------------*/

/***************************************************************END OF FILE****/
