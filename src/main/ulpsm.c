
/**
 * @file ulpsm.c
 * @version 1.0
 * @date 22/07/2024
 * @author: Andres Machtey
 *  https://www.alldatasheet.com/datasheet-pdf/pdf/1179026/ETC2/SSD1306.html
 */

#include "ulpsm.h"
#include "esp_log.h"
#include "driver/adc.h"

#define ULPSM_PIN ADC1_CHANNEL_4 // Por ejemplo, GPIO32
#define VREF_PIN ADC1_CHANNEL_5  // Por ejemplo, GPIO33

static const char *TAG = "ULPSM";

// Función de inicialización del ULPSM
void ulpsm_init(ulpsm_config_t *config)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ULPSM_PIN, ADC_ATTEN_DB_2_5);
    // adc1_config_channel_atten(VREF_PIN, ADC_ATTEN_DB_0);
    config->v_offset = 0.0f;
    ESP_LOGI(TAG, "ULPSM initialized with sensitivity code: %f and TIA gain: %f", config->sensitivity_code, config->tia_gain);
}

// Función para leer un valor del ADC utilizando esp_adc
uint32_t read_adc_value3(adc1_channel_t channel)
{
    // Configurar la precisión y la atenuación del canal ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, ADC_ATTEN_DB_0);

    // Leer el valor RAW del ADC
    uint32_t adc_reading = adc1_get_raw(channel);

    return adc_reading;
}

// Función para calcular la concentración de gas
float calculate_gas_concentration(uint16_t vgas_raw, uint16_t vref_raw, ulpsm_config_t *config)
{
    float vgas = (float)vgas_raw / 4096.0f * 3.3f; // Convertir el valor RAW del ADC a voltaje
    float vref = (float)vref_raw / 4096.0f * 3.3f; // Convertir el valor RAW del ADC a voltaje
    float vgas0 = vref + config->v_offset;
    float concentration = (vgas - vgas0) / (config->sensitivity_code * config->tia_gain);
    return concentration;
}
