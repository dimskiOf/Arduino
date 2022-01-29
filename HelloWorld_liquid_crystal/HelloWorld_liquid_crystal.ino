//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  Serial.begin(9600);
  lcd.begin();
}


void loop()
{
  lcd.setCursor(0,0);
  lcd.print("TEST LCD i2C");
  lcd.setCursor(0,1);
  lcd.print("KelasRobot.com");
}
