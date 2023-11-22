/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Include library header files ----------------------------------------------*/
/* Include peripheral header files -------------------------------------------*/
#include "pico/stdlib.h"
#include "hardware/adc.h"

/* Include On-board hardware library header files ----------------------------*/
// #include "rp_eth_io_pins.h"
#include "wizchip_conf.h"

/* Include user header files -------------------------------------------------*/
#include "rp_eth_io_common.h"

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
wiz_NetInfo g_net_info =
{
    .mac = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},//{0x80, 0x1F, 0x12, 0x32, 0x87, 0xA5}, // MAC address
    .ip = {192, 168, 0, 100},                     // IP address
    .sn = {255, 255, 255, 0},                    // Subnet Mask
    .gw = {192, 168, 0, 1},                     // Gateway
    .dns = {8, 8, 8, 8},                         // DNS server
    .dhcp = NETINFO_DHCP                         // DHCP enable/disable
};
wiz_PhyConf g_phyconf = {
	.by     = PHY_CONFBY_SW,
    .mode   = PHY_MODE_MANUAL,//PHY_MODE_AUTONEGO
	.speed  = PHY_SPEED_100,
	.duplex = PHY_DUPLEX_HALF//PHY_DUPLEX_FULL
};
wiz_NetTimeout g_netTimeout;
uint8_t rp_eth_io_mac_addr[6] = {0xFF};

/* Imported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Exported functions prototypes----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/***************************************************************END OF FILE****/
