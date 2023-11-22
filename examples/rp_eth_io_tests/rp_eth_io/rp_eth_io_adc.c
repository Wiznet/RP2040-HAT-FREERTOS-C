/* Include system header files -----------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Include library header files ----------------------------------------------*/
/* Include peripheral header files -------------------------------------------*/
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"

/* Include On-board hardware library header files ----------------------------*/
#include "rp_eth_io_pins.h"

/* Include user header files -------------------------------------------------*/
#include "rp_eth_io_adc.h"
#include "rp_eth_io_common.h"


/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define ADC_AVDD            2.875f
#define ADC_AMP_FACTOR      -8.0f
#define ADC_OFFSET_FACTOR   0.5f

/* Exported variables --------------------------------------------------------*/
/* Imported variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static const float conversion_factor = ADC_AMP_FACTOR / (float)(1 << 12) * ADC_AVDD;
static const float offset = -1.0f * ADC_AMP_FACTOR * ADC_OFFSET_FACTOR * ADC_AVDD;

/* Exported functions prototypes----------------------------------------------*/
void rp_eth_io_getADCvalue(uint8_t, float, uint32_t, float *);


/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void rp_eth_io_getADCvalue(uint8_t ch, float sample_hz, uint32_t data_points, float *voltage)
{
    int pin;
    uint16_t *dat_dma = (uint16_t *)voltage;

    switch (ch) {
        case 0:
            pin = AN0_PIN;
            break;
        case 1:
            pin = AN1_PIN;
            break;
        case 2:
            pin = AN2_PIN;
            break;
        case 3:
            pin = AN3_PIN;
            break;
        default:
            printf("Invalid ch: %d\n", ch);
            return;
    }

    adc_gpio_init(pin);
    adc_init();
    adc_select_input(ch);

    adc_fifo_setup(
        true,   // Write each completed conversion to the sample FIFO
        true,   // Enable DMA data request (DREQ)
        1,      // DREQ (and IRQ) asserted when at least 1 sample present
        false,  // We won't see the ERR bit because of 8 bit reads; disable.
        false   // Shift each sample to 8 bits when pushing to FIFO
    );
    float sampling_clkdiv = 48000000.0 / sample_hz - 1.0;
    adc_set_clkdiv(sampling_clkdiv);

    // DMA
    int dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(dma_chan, &cfg,
                          dat_dma,        // dst
                          &adc_hw->fifo,  // src
                          data_points,    // transfer count
                          true            // start immediately
    );

    // printf("Starting capture\r\n");
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_chan);
    // printf("Capture finished\n");
    adc_run(false);
    adc_fifo_drain();
    dma_channel_unclaim(dma_chan);

    for (int i = data_points - 1; i >= 0; i--) {
        *(voltage + i) = (float)*(dat_dma + i) * conversion_factor + offset;
    }
}


/* Private functions ---------------------------------------------------------*/

/***************************************************************END OF FILE****/
