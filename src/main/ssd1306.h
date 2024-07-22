
/**
 * @file ssd1306.h
 * @version 1.0
 * @date 22/07/2024
 * @author: Andres Machtey
 *  source:
 *  https://www.alldatasheet.com/datasheet-pdf/pdf/1179026/ETC2/SSD1306.html
 *  @addtogroup ssd 
 * @{
 */

#ifndef SSD1306_H
#define SSD1306_H

#include "esp_err.h"

// Definición de pines y parámetros del I2C
#define I2C_MASTER_SCL_IO 19
#define I2C_MASTER_SDA_IO 18
#define I2C_MASTER_NUM I2C_NUM_1
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define SSD1306_I2C_ADDR 0x3C
#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

#define CHARS_COLS_LENGTH 8 // Número de columnas que ocupa un carácter

// Dimensiones del display
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Búfer de la pantalla
extern uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

// Funciones de inicialización
void i2c_master_init();

/**
 * @brief Initialize the SSD1306 OLED display with a sequence of commands.
 *
 * This function sends a series of initialization commands to the SSD1306 OLED
 * display controller to set it up for normal operation.
 */
void ssd1306_init(void);

/**
 * @brief Clears the SSD1306 display by setting all pixels to 0.
 *
 * This function sets the SSD1306 display into horizontal addressing mode and clears each page by
 * writing 128 bytes of 0s. It verifies the data written and corrects any pixels that were not
 * properly cleared.
 *
 * The SSD1306 display is organized into 8 pages, with each page containing 128 columns. This
 * function iterates through each page and column, ensuring that all pixels are turned off.
 */
void ssd1306_clear_display(void);

/**
 * @brief Draw a character on the SSD1306 display.
 *
 * @param c Character to draw.
 * @param x Horizontal position.
 * @param y Vertical position.
 */

void ssd1306_draw_char(char c, int x, int y);

/**
 * @brief Draw a string on the SSD1306 display.
 *
 * @param str Pointer to the null-terminated string to draw.
 * @param x Horizontal position.
 * @param y Vertical position.
 */

void ssd1306_draw_string(const char *str, int x, int y);

/**
 * @brief Read data from the SSD1306 display over I2C.
 *
 * @param data Pointer to the buffer where the read data will be stored.
 * @param size Number of bytes to read.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */

esp_err_t ssd1306_read_data(uint8_t *data, size_t size);

#endif // SSD1306_H
/*@*/