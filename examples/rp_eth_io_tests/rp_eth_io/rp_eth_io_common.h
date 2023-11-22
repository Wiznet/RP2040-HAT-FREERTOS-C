/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RP_ETH_IO_COMMON_H
#define __RP_ETH_IO_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* Include library header files ----------------------------------------------*/
/* Include peripheral header files -------------------------------------------*/
#include "wizchip_conf.h"

/* Include user header files -------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define TEMP_VREF        2.875f   // Voltage of Vref

/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern wiz_NetInfo g_net_info;
extern wiz_PhyConf g_phyconf;
extern wiz_NetTimeout g_netTimeout;
extern uint8_t rp_eth_io_mac_addr[];

/* Exported function prototypes ----------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __RP_ETH_IO_COMMON_H */
/***************************************************************END OF FILE****/
