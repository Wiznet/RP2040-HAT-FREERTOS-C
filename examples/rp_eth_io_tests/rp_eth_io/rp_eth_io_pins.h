/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RP_ETH_IO_PINS_H
#define __RP_ETH_IO_PINS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
/* Include user header files -------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
// On-board hardware
#define STATUS_LED_PIN          25
#define IO_EN_PIN               22

// RS-485
#define RP_ETH_IO_UART_PORT     uart0
#define RS485_TX_PIN            0
#define RS485_RX_PIN            1
#define RS485_CTRL_PIN          7
#define DEFAULT_UART_BAUD_RATE  9600

// I2C
#define RP_ETH_IO_I2C_PORT      i2c0
#define DEFAULT_I2C_BAUD_RATE   100000
#define SDA_PIN                 8
#define SCL_PIN                 9

// NPN Output pins
#define NO0_PIN                 14
#define NO1_PIN                 15
#define NO2_PIN                 16
#define NO3_PIN                 17

// NPN Input pins
#define NI0_PIN                 18
#define NI1_PIN                 19
#define NI2_PIN                 20
#define NI3_PIN                 21

// GPIO pins
#define IO0_PIN                 10
#define IO1_PIN                 11
#define IO2_PIN                 12
#define IO3_PIN                 13

// Analog input pins
#define AN0_PIN                 26
#define AN1_PIN                 27
#define AN2_PIN                 28
#define AN3_PIN                 29


/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern const int npn_input_pins[], npn_output_pins[], io_pins[];

/* Exported function prototypes ----------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __RP_ETH_IO_PINS_H */

/***************************************************************END OF FILE****/
