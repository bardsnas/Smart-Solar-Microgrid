#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Confirmed address

// Task handles
TaskHandle_t lcdTask_Handle = NULL;
TaskHandle_t pvCell_Handle = NULL;

// Semaphore
SemaphoreHandle_t pvSemaphore = NULL;

// Pins
const int voltagePin = 7;
const int currentPin = 15;

// Shared data
volatile float voltage = 0.0;
volatile float current = 0.0;
volatile float power = 0.0;

const float sensitivity = 0.185; // ACS712 sensitivity
const float offset = 2.5;

void pvCell(void *pvParameter) {
  while (1) {
    if (xSemaphoreTake(pvSemaphore, portMAX_DELAY) == pdTRUE) {
      // Simulated analog read (replace with real ADC reads)
      int rawV = analogRead(voltagePin);
      float sensorVoltage = rawV * (3.3 / 4095.0);
      float vIn = sensorVoltage * (25.0 / 3.3);  // V025C scale

      int rawC = analogRead(currentPin);
      float cADC = rawC * (3.3 / 4095.0);
      float iIn = (cADC - offset) / sensitivity;

      voltage = vIn;
      current = iIn;
      power = vIn * iIn;

      xSemaphoreGive(pvSemaphore);
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void lcdTask(void *pvParameter) {
  while (1) {
    if (xSemaphoreTake(pvSemaphore, portMAX_DELAY) == pdTRUE) {
      lcd.setCursor(0, 0);
      lcd.print("V:");
      lcd.print(voltage, 1);
      lcd.print(" I:");
      lcd.print(current, 2);

      lcd.setCursor(0, 1);
      lcd.print("P:");
      lcd.print(power, 1);
      lcd.print("W    ");  // Clear remaining chars

      xSemaphoreGive(pvSemaphore);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  delay(100); // Ensure LCD is stable before tasks

  pvSemaphore = xSemaphoreCreateBinary();
  xSemaphoreGive(pvSemaphore);

  xTaskCreate(pvCell, "pvCell", 2048, NULL, 2, &pvCell_Handle);
  xTaskCreate(lcdTask, "lcdTask", 2048, NULL, 1, &lcdTask_Handle);
}

void loop() {
  // Empty. FreeRTOS runs tasks.
}
