/**
 * \mainpage Trabajo Arquitectura y Programación de Sistemas Embebidos
 * \section Introducción
 * Este programa es parte de los objetivos del curso de programación de dispositivos embebidos en la Especialización en Sistemas Embebidos, Cohorte 2024.
 * El programa calcula la concentracion de ozono a partir del sensor ulpsm y lo muestra en una pantalla ssd1306 Ole,
 * se inicia la cuenta de tiempo cuando se supera un valor de concentracion y se muestra en pantalla tiempo, cuando es alcanzado este tiempo, con la concentracion 
 * por encima de un valor, se termina el proceso, con un mensaje que se alcanzo el tiempo y se procederia a cortar el generador de ozono.
 
 
 * \section changes Versiones y Cambios
 *
 * Fecha          | Descripción
 * -------------- | -------------------------
 * 10/06/2024     | Versión inicial
 * 22/04/2024     | Modificado y corregido
 *
 * \author Andrés Machtey
 * \version 1.0
 * \date 22/06/2024
 */


#include <stdio.h>
#include "ssd1306.c"
#include "ulpsm.h"

// Configuración del ULPSM
ulpsm_config_t ulpsm_config = {
    .sensitivity_code = 1, // on lebel of sensor
    .tia_gain = -5.5,      // TIa gain for ozono 449
    .v_offset = 0.0};      // could be -25mv or 0 not so sure!!!!

void app_main()
{
  i2c_master_init();
  ssd1306_init();
  ulpsm_init(&ulpsm_config);

  ssd1306_init();          // Inicializar la pantalla SSD1306
  ssd1306_clear_display(); // Limpiar la pantalla
  // Variables para la concentración y el tiempo objetivo
  float target_concentration;
  int target_time;
  int current_time = 0;

  // Solicita la concentración objetivo y el tiempo desde la consola
  printf("Ingrese la concentración objetivo: \n");
  scanf("%f", &target_concentration);
  printf("Ingrese el tiempo objetivo en segundos: \n ");
  scanf("%d", &target_time);
  target_time = 10;
  ssd1306_draw_string("Measure ozone!", 0, 5);

  vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos
  target_concentration = 0.2;
  printf("Tiempo objetivo ingresado: %d segundos\n", target_time);
  // printf("concentración: %ld \n", concentration);

  ssd1306_clear_display();
  while (1)
  {
    // Leer y calcular concentración
    uint32_t vgas_raw = read_adc_value3(ULPSM_PIN);
    uint32_t vref_raw = read_adc_value3(VREF_PIN);
    float concentration = calculate_gas_concentration(vgas_raw, vref_raw, &ulpsm_config);
    // Mostrar concentración y tiempo restante
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "Conc: %.2f", concentration);
    
    ssd1306_clear_display();
    
    ssd1306_draw_string(buffer, 0, 0);
    //---------------
    if (concentration >= target_concentration)
    {
      printf("Concentración: %.2f\n", concentration);
      current_time++;
      snprintf(buffer, sizeof(buffer), "Tiempo: %d", target_time - current_time);
      ssd1306_draw_string(buffer, 0, 20);

      if (current_time >= target_time)
      {

        ssd1306_clear_display();
        //ssd1306_clear_buffer();
        ssd1306_draw_string("Tiempo alcanzado", 0, 30);

        break;
      }
    }
    else
    {
      current_time = 0;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  //----------------
}