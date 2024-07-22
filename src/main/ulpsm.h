
/** 
 * @file ulpsm.h
 * @version 1.0
 * @date 22/07, 2024
 * @author: Andres Machtey
 *  source:
 *  https://www.alldatasheet.com/datasheet-pdf/pdf/1179026/ETC2/SSD1306.html
 * *  @addtogroup ULPSM 
 * @{
 */
#ifndef ULPSM_H
#define ULPSM_H

#include "driver/adc.h"



#define ULPSM_PIN ADC1_CHANNEL_0  // Ajusta según tu configuración de hardware
#define VREF_PIN ADC1_CHANNEL_1   // Ajusta según tu configuración de hardware

/**
 * @brief struct sensor ozone 
 */
typedef struct {
    float sensitivity_code;
    float tia_gain;
    float v_offset;
} ulpsm_config_t;


/**
 * @brief Initializes the ULPSM with the given configuration.
 *  
 * This function configures the ADC width and attenuation for the ULPSM pin,
 * sets the voltage offset to zero, and logs the initialization parameters.
 *
 * @param config Pointer to the ULPSM configuration structure.
 */

void ulpsm_init(ulpsm_config_t *config);
/**
 * @brief Reads a value from the ADC using the esp_adc library.
 *
 * This function configures the ADC width and attenuation for the specified channel,
 * reads the raw ADC value, and returns it.
 *
 * @param channel The ADC channel to read from.
 * @return The raw ADC value.
 */
uint32_t read_adc_value3(adc1_channel_t channel);

/**
 * @brief Calculates the gas concentration from raw ADC values.
 *
 * This function converts the raw ADC values to voltages, applies an offset,
 * and calculates the gas concentration using the specified sensitivity code and TIA gain.
 *
 * @param vgas_raw The raw ADC value for the gas sensor.
 * @param vref_raw The raw ADC value for the reference voltage.
 * @param config The configuration parameters for the ULPSM, including sensitivity code, TIA gain, and voltage offset.
 * @return The calculated gas concentration.
 */

float calculate_gas_concentration(uint16_t vgas_raw, uint16_t vref_raw, ulpsm_config_t *config);

#endif // ULPSM_H
 