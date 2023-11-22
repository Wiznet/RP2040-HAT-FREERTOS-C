/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RP_ETH_IO_I2C_H
#define __RP_ETH_IO_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Include user header files -------------------------------------------------*/
#define MAC_24AA02Exx_DEV_ADDR        0x50
#define MAC_24AA02Exx_REG_ADDR_MAC    0xF8

/* Exported macro ------------------------------------------------------------*/
/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void rp_eth_io_i2c_dev_scan(void);
extern bool reserved_addr(uint8_t);
extern uint8_t *get_mac_address(uint8_t);
extern void i2c_read_registers(uint8_t, uint8_t, uint8_t *, uint16_t);

#ifdef __cplusplus
}
#endif

#endif /* __RP_ETH_IO_I2C_H */
/***************************************************************END OF FILE****/
