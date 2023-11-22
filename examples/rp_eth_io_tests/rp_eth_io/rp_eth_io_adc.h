/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RP_ETH_IO_ADC_H
#define __RP_ETH_IO_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

/* Include user header files -------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern void rp_eth_io_getADCvalue(uint8_t, float, uint32_t, float *);

#ifdef __cplusplus
}
#endif

#endif /* __RP_ETH_IO_ADC_H */
/***************************************************************END OF FILE****/
