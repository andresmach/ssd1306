
/**
 * @file ssd1306.c
 * \version 1.0
 * \date 22/07/2024
 * \author: Andres Machtey
 *  https://www.alldatasheet.com/datasheet-pdf/pdf/1179026/ETC2/SSD1306.html
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/i2c.h"
#include "driver/adc.h"
#include "ssd1306.h"
#include "font8.h"
#include <math.h>

// Búfer de la pantalla
uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    esp_err_t res = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (res != ESP_OK)
    {
        printf("Error in i2c_param_config: %s\n", esp_err_to_name(res));
    }
    res = i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (res != ESP_OK)
    {
        printf("Error in i2c_driver_install: %s\n", esp_err_to_name(res));
    }
}




/**
 * @brief Write a series of commands to the SSD1306 display.
 *
 * This function sends a sequence of commands to the SSD1306 OLED display
 * using the I2C protocol.
 *
 * @param commands Pointer to an array of commands to be sent.
 * @param size Number of commands in the array.
 * @return esp_err_t Returns ESP_OK on success or an error code on failure.
 */

esp_err_t ssd1306_write_commands(uint8_t *commands, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SSD1306_COMMAND, true);
    for (size_t i = 0; i < size; i++)
    {
        i2c_master_write_byte(cmd, commands[i], true);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t ssd1306_write_data(uint8_t *data, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, SSD1306_DATA, true);
    i2c_master_write(cmd, data, size, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void ssd1306_clear_display(void)
{
    // Comando para configurar el modo de dirección horizontal
    uint8_t set_horizontal_addressing_mode[] = {0x20, 0x00};
    ssd1306_write_commands(set_horizontal_addressing_mode, sizeof(set_horizontal_addressing_mode));

    uint8_t clear_data[128] = {0}; // Tamaño adecuado para una página (128 bytes)

    for (uint8_t page = 0; page < 8; page++)
    {
        uint8_t page_address_commands[] = {
            0xB0 | page, // Set Page Start Address for Page Addressing Mode
            0x00,        // Set Lower Column Start Address for Page Addressing Mode
            0x10         // Set Higher Column Start Address for Page Addressing Mode
        };
        ssd1306_write_commands(page_address_commands, sizeof(page_address_commands));
        ssd1306_write_data(clear_data, sizeof(clear_data));

        // Verificar y corregir el contenido escrito
        uint8_t verify_data[128] = {0};
        ssd1306_write_commands(page_address_commands, sizeof(page_address_commands));
        ssd1306_read_data(verify_data, sizeof(verify_data)); // Leer datos de la pantalla

        // Corregir cualquier pixel no limpiado correctamente
        bool corrected = false;
        for (int i = 0; i < sizeof(verify_data); i++)
        {
            if (verify_data[i] != 0)
            {
                // printf("Correcting pixel at page %d, column %d\n", page, i);
                verify_data[i] = 0; // Corregir el pixel a 0
                corrected = true;
            }
        }

        // Si se corrigieron datos, escribir nuevamente en la pantalla
        if (corrected)
        {
            ssd1306_write_commands(page_address_commands, sizeof(page_address_commands));
            ssd1306_write_data(verify_data, sizeof(verify_data));
        }
    }
}

void ssd1306_init(void)
{
    uint8_t init_commands[] = {
        0xAE, // Display Off
        0xD5, // Set Display Clock Divide Ratio/Oscillator Frequency
        0x80, // Suggested ratio
        0xA8, // Set Multiplex Ratio
        0x3F, // 1/64 duty
        0xD3, // Set Display Offset
        0x00, // No offset
        0x40, // Set Start Line
        0x8D, // Charge Pump Setting
        0x14, // Enable charge pump
        0x20, // Memory Addressing Mode
        0x00, // Horizontal addressing mode
        0xA1, // Set Segment Re-map
        0xC8, // Set COM Output Scan Direction
        0xDA, // Set COM Pins Hardware Configuration
        0x12,
        0x81, // Set Contrast Control
        0xCF,
        0xD9, // Set Pre-charge Period
        0xF1,
        0xDB, // Set VCOMH Deselect Level
        0x40,
        0xA4, // Entire Display ON (Resume)
        0xA6, // Normal Display
        0xAF  // Display ON
    };

    ssd1306_write_commands(init_commands, sizeof(init_commands));
}

void ssd1306_draw_char(char c, int x, int y)
{
    if (c < 0x20 || c > 0x7F)
    {
        c = 0x20; // carácter inválido, usar espacio
    }
    uint8_t index = c - 0x20;
    uint8_t char_data[CHARS_COLS_LENGTH];

    for (uint8_t i = 0; i < CHARS_COLS_LENGTH; i++)
    {
        char_data[i] = FONTS[index][i];
    }

    uint8_t column_address_commands[] = {
        0x21,                     // Set Column Address     //0x21, columna_inicio, columna_final
        x,                        // Start column address
        x + CHARS_COLS_LENGTH - 1 // End column address
    };
    ssd1306_write_commands(column_address_commands, sizeof(column_address_commands));

    uint8_t page_address_commands[] = {
        0x22, // Set Page Address
        y,    // Start page address
        y     // End page address
    };
    ssd1306_write_commands(page_address_commands, sizeof(page_address_commands));

    ssd1306_write_data(char_data, CHARS_COLS_LENGTH);
}

void ssd1306_draw_string(const char *str, int x, int y)
{
    // Dibujar cada carácter de la cadena
    while (*str)
    {
        ssd1306_draw_char(*str++, x, y); // Dibujar el carácter actual y avanzar el puntero de la cadena
        x += CHARS_COLS_LENGTH;          // Avanzar la posición horizontal para el siguiente carácter
    }
}

esp_err_t ssd1306_read_data(uint8_t *data, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Configuración para leer datos desde la pantalla SSD1306
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true); // Dirección de escritura
    i2c_master_write_byte(cmd, SSD1306_DATA, true);                               // Comando de datos (modo de datos)
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);

    if (ret != ESP_OK)
    {
        i2c_cmd_link_delete(cmd);
        return ret;
    }

    // Ahora configuramos para leer los datos
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_READ, true); // Dirección de lectura
    i2c_master_read(cmd, data, size, I2C_MASTER_LAST_NACK);                      // Leer datos
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}
