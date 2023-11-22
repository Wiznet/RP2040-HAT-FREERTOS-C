/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TESTS_H
#define __TESTS_H

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
extern void npn_input_test(void);
extern void npn_output_test(void);
extern void io_test(void);
extern void adc_test(void);
extern void rs485_test(void);
extern void i2c_test(void);
extern void udp_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_H */
/***************************************************************END OF FILE****/
