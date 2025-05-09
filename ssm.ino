#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    // Initialize LCD display
    lcd.init();
    lcd.backlight();
}

void loop() {

}