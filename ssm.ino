#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Create Handles
TaskHandle_t lcdTask_Handle = NULL;
TaskHandle_t pvCell_Handle = NULL;
SemaphoreHandle_t pvSemaphore = NULL;

// Function prototype
void lcdTask(void *pvParameter);
void pvCell(void *pvParameter);

void setup() {
    // Initialize LCD display
    lcd.init();
    lcd.backlight();
    // Create binary semaphore for synchronizing PV Cell production data.
    pvSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(pvSemaphore);
    // Create the `Light Detector Task`
    xTaskCreate(pvCell, "pvCell", 1024, NULL, 1, &pvCell_Handle);
    // Create `LCD Task`
    xTaskCreate(lcdTask, "lcdTask", 1024, NULL, 1, &lcdTask_Handle);
}

void loop() {

}

void pvCell(void *pvParameter) {
    while (1) {
        if (xSemaphoreTake(pvSemaphore, portMAX_DELAY) == pdTRUE) {
            
            xSemaphoreGive(pvSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void lcdTask(void *pvParameter) {

}