#include <EEPROM.h>


char example_string[] = "~New eeprom string";

const int eeprom_size = 4096; // values saved in eeprom should never exceed 500 bytes
char eeprom_buffer[eeprom_size];

char first_eeprom_value;


void setup() {

  // clear eeprom on new flash... just for debugging
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  
  Serial.begin(9600); // serial communication initialize
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  
}

void loop() {
  
  delay(3000);
  
  first_eeprom_value = EEPROM.read(0);
  
  if(first_eeprom_value != '~'){
  
    Serial.println("No value saved in eeprom... Commencing save");
  
    save_string_to_eeprom(example_string);
  
  
  } else {
    
    Serial.println("Eeprom value exists... Dishing it out");
    
    read_string_from_eeprom(eeprom_buffer);
    
    Serial.println(eeprom_buffer);

    Serial.print("completed eeprom_buffer length: ");
    Serial.print(strlen(eeprom_buffer));
    Serial.println();
  
  }
  
}

void save_string_to_eeprom(char *stringIn){

  Serial.print("stringIn length: ");
  Serial.print(strlen(stringIn));
  Serial.println();
  
  for(int i = 0; i < strlen(stringIn); i++){
    
      EEPROM.write(i, stringIn[i]);
    
  }

}

void read_string_from_eeprom(char *bufferIn){

  Serial.print("bufferIn sizeof: ");
  Serial.print(sizeof(bufferIn));
  Serial.println();
  
  for(int i = 0; i < eeprom_size; i++){
  
    bufferIn[i] = EEPROM.read(i);
  
  }

}
