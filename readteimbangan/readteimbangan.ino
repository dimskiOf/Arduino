#include <EEPROM.h>

String temp;
uint8_t newStr4;
int address4 = 5;

void setup() {
  if(EEPROM.read(address4) == 60){
     newStr4 = SERIAL_7N1 ;
    }else if(EEPROM.read(address4) == 61){
      newStr4 = SERIAL_7N2 ;
    }else if(EEPROM.read(address4) == 62){
      newStr4 = SERIAL_8N1 ;
    }else if(EEPROM.read(address4) == 63){
      newStr4 = SERIAL_8N2 ;
    }else{
      newStr4 = SERIAL_8N1 ;             
    }
  Serial.begin(19200);
  Serial1.begin(19200,newStr4);

  Serial.print(newStr4);

}

void loop() {
  while (Serial1.available() > 0){
    temp += char(Serial1.read());
  }
  temp.trim();
  Serial.print(temp);
  temp = "";
}
