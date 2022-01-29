#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const byte lcdAddr = 0x27;  // Address of I2C backpack
const byte lcdCols = 16;    // Number of character in a row
const byte lcdRows = 2;     // Number of lines
LiquidCrystal_I2C lcd(lcdAddr, lcdCols, lcdRows);

int p, minX;

int cursorColumn = 0;
int j = 0;
char minus[100] = "!0)!#)";
char minustampil[100] = "";
String arrDatas;

const char message[] ="PT. MAHAJAYA PLASTINDO INDONESIA";

//start keypad
bool change = false;
bool change2 = false;
bool change3 = false;

const byte ROWS = 4; 
const byte COLS = 4; 
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {28, 30, 32, 34}; 
byte colPins[COLS] = {36, 38, 40, 42}; 

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

bool eventhandler = true;
//end keypad


//start usb
String DataBarcode;
bool x = false;

const uint8_t numKeys[10] PROGMEM = {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')'};
const uint8_t symKeysUp[12] PROGMEM = {'_', '+', '{', '}', '|', ':', '"', '<', '>', '?'};
const uint8_t symKeysLo[12] PROGMEM = {'-', '=', '[', ']', '\\', ' ', ';', '\'', '`', ',', '.', '/'};
const uint8_t padKeys[5] PROGMEM = {'/', '*', '-', '+', '\r'};
bool shift = false;
class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
//  if (buf[2] == 1 || buf[2] == 0) {
//    return;
//  }

  if(buf[0] == 2)
      shift = true;
  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(shift, buf, buf[i]);
      shift = false;
    }

    return;
  }
}

uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x01d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    if (shift)
      return ((uint8_t)pgm_read_byte(&numKeys[key - 0x1e]));
    else
      return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');

  }
  // Keypad Numbers
  else if (VALUE_WITHIN(key, 0x59, 0x61)) {
    return (key - 0x59 + '1');
  } else if (VALUE_WITHIN(key, 0x2d, 0x38))
    return ((upper) ? (uint8_t)pgm_read_byte(&symKeysUp[key - 0x2d]) : (uint8_t)pgm_read_byte(&symKeysLo[key - 0x2d]));
  else if (VALUE_WITHIN(key, 0x54, 0x58))
    return (uint8_t)pgm_read_byte(&padKeys[key - 0x54]);
  else {
    switch (key) {
      case UHS_HID_BOOT_KEY_SPACE: return (0x20);
      case UHS_HID_BOOT_KEY_ENTER: return ('\r'); // Carriage return (0x0D)
    }
  }
  //Serial.println(key, HEX);
  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  //Serial.print(upper);
  uint8_t ascii = KeyToAscii(upper, mod, key);
  //Serial.print((char)ascii);
  if (ascii){
    x = true;
    DataBarcode = (char)ascii;
  }
}

void MyParser::OnScanFinished() {
  Serial.println();
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

//end usb

unsigned long previousMillis = 0;
const long interval = 0;
const long interval2 = 5000;

String arrData[1];
String timbangan;
String a = "";
String b ="";
String newStr3;
String newStr4;
uint8_t newStr5;
String newStr6;
String newStr7;
String datatoread;
String tampung;
int address = 2;
int address2 = 3;
int address3 = 4;
int address4 = 5;
int value = 0;

bool y = true;

String kirimset;

const unsigned int scrollDelay = 500;   // Miliseconds before scrolling next char
const unsigned int demoDelay = 2000; 
byte textLen;

void setup() {
 int setter = EEPROM.read(200) * 100;
 if(EEPROM.read(address4) == 60){
     newStr5 = SERIAL_7N1 ;
    }else if(EEPROM.read(address4) == 61){
      newStr5 = SERIAL_7N2 ;
    }else if(EEPROM.read(address4) == 62){
      newStr5 = SERIAL_8N1 ;
    }else if(EEPROM.read(address4) == 63){
      newStr5 = SERIAL_8N2 ;
    }else{
      newStr5 = SERIAL_7N1 ;             
    }
Serial.begin(19200);
Serial1.begin(setter,newStr5);
Serial2.begin(19200);   

  textLen = sizeof(message) - 1;
  lcd.init();
  lcd.backlight();
  gettextfromnode("","");
  
customKeypad.addEventListener(keypadEvent);

//inisialisasi usb
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Parser);
  
}



void loop() {
 Usb.Task();

if (eventhandler == true){
  getmaster();
}
  
  if (change == true){ 
    getnum();
  }

    if (change2 == true){ 
    getnum2();
  }

  if (change3 == true){
    getnum3();
  }
  
  while (Serial2.available() > 0){
  tampung += char(Serial2.read());
 // tampung.trim();
  gettextfromnode(tampung,"");
  }
  tampung = "";

  
  
  if (y){
    if(EEPROM.read(address3) == 50){
     newStr4 = "PT. MPI" ;
    }else if(EEPROM.read(address3) == 51){
      newStr4 = "GD 99 COMPOUND" ;
    }else if(EEPROM.read(address3) == 52){
      newStr4 = "GD 99 INJECTION" ;
    }else if(EEPROM.read(address3) == 53){
      newStr4 = "PT. OMI" ;
    }else{
      newStr4 = "NO SETTING" ;             
    }
  
    gettextfromnode("",newStr4);
  }
  
  if (EEPROM.read(address) == 1){ 
    kirimdata();
    
    }else if(EEPROM.read(address) == 2){
     
    
    }
  
}

void gettextfromnode(String isinya,String isinya2){
int setter2 = EEPROM.read(200);
 if (setter2 == 0){
  EEPROM.write(200, 192);
  int setter2 = EEPROM.read(200);      
 }else{
  int setter2 = EEPROM.read(200);
 }
 if(EEPROM.read(address4) == 60){
     newStr6 = "7N1";
    }else if(EEPROM.read(address4) == 61){
      newStr6 = "7N2";
    }else if(EEPROM.read(address4) == 62){
      newStr6 = "8N1";
    }else if(EEPROM.read(address4) == 63){
      newStr6 = "8N2";
    }else{
      newStr6 = "7N1";             
    }
 if(EEPROM.read(address2) == 40){
  newStr3 = "RM KELUAR";
 }else if(EEPROM.read(address2) == 41){
  newStr3 = "RM MASUK";
 }else if(EEPROM.read(address2) == 42){
  newStr3 = "FG KELUAR";
 }else if(EEPROM.read(address2) == 43){
  newStr3 = "FG MASUK";
 }else if(EEPROM.read(address2) == 44){
  newStr3 = "BRG SISA";
 }else{
  newStr3 = "NO SETTING";
 }
 //lcd.clear();
 lcd.backlight();
 lcd.setCursor(0,0);
 lcd.print(newStr3 + " " + newStr6 + setter2);// waktu
 if (isinya != ""){
 lcd.clear();
 lcd.setCursor(1, 1);
  // Display static text
 lcd.print(isinya);
 delay(250);
 y = true;
 }else{
 lcd.setCursor(1, 1);
  // Display static text
 lcd.print(isinya2); 
 }
}

void keypadEvent(KeypadEvent key){
    switch (customKeypad.getState()){
    case PRESSED:
       if(key == '#'){
          change2 = true;
        }else if(key == '*'){
          change = true;
        }
        break;
    }
}

void getmaster() {
  
  char key = customKeypad.getKey();

      switch (key) {
         case 'A':
           kirimset = "500";
           lcd.clear();
           Serial2.println(kirimset);
           EEPROM.write(address, 1);
           EEPROM.write(address2,40);
           gettextfromnode("","");
           break;
        case 'B':
           kirimset = "501";
           lcd.clear();
           Serial2.println(kirimset);
           EEPROM.write(address, 1);
           EEPROM.write(address2,41);
            gettextfromnode("","");
         break;
        case 'C':
           kirimset = "502";
           lcd.clear();
           Serial2.println(kirimset);
          EEPROM.write(address, 1);
          EEPROM.write(address2,42);
           gettextfromnode("","");
        break;
        case 'D':
           kirimset = "503";
           lcd.clear();
           Serial2.println(kirimset);
          EEPROM.write(address, 1);
           EEPROM.write(address2,43);
           gettextfromnode("","");
        break;
        case '*':
          change = true;
          eventhandler = false;
        break;
        case '#':
          change2 = true;
        break;
    }
 
}

void kirimdata()
{  
arrDatas = EEPROM.get(0x0010, minustampil);
 if (x){
        Serial.print(arrDatas);
       while (Serial1.available() > 0){ 
        a = char(Serial1.read());
        a.trim();
        Serial.println('$'+a+'*');
       }
       x = false;
       y = false;          
       Serial.print("#");  
       Serial.print(DataBarcode);
       Serial.print("@");
       DataBarcode = "";
      }
      if(x == false){ 
     unsigned long currentMillis = millis();
      if(currentMillis - previousMillis >= interval2)
      {
       previousMillis = currentMillis;
       //y = true;
      }
      }
}


int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}
int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\ 0'; // !!! NOTE !!! Remove the space between the slash "/" and "0" (I've added a space because otherwise there is a display bug)
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

void(* resetFunc) (void) = 0;

void getnum() {
  
  char key = customKeypad.waitForKey();

         switch (key) {
           case NO_KEY:
             // Wait and then tell user they can start the Serial Monitor and type in characters to
             break;
                               
            case '0':
              lcd.clear();
              EEPROM.write(200, 12);
              eventhandler = true;
              change = false;
            break;
                   
            case '1':
              lcd.clear();
              EEPROM.write(200, 24);
              eventhandler = true;
              change = false;
             break;

             case '2':
             lcd.clear();
              EEPROM.write(200, 48);
              eventhandler = true;
              change = false;
             break;

             case '3':
             lcd.clear();
              EEPROM.write(200, 96);
              eventhandler = true;
              change = false;
              break;

             case '4':
             lcd.clear();
              EEPROM.write(200, 192);
              eventhandler = true;
              change = false;
             break;

             case '5':
             lcd.clear();
             EEPROM.write(address4, 60);
             eventhandler = true;
              change = false;
             break;

              case '6':
              lcd.clear();
              EEPROM.write(address4, 61);
              eventhandler = true;
              change = false;
             break;

              case '7':
              lcd.clear();
              EEPROM.write(address4, 62);
              eventhandler = true;
              change = false;
             break;

              case '8':
              lcd.clear();
              EEPROM.write(address4, 63);
              eventhandler = true;
              change = false;
             break;

              case '9':
              resetFunc();
              eventhandler = true;
              change = false;
             break;

             case 'A':
             kirimset = "507";
             lcd.clear();
             Serial2.println(kirimset);
             EEPROM.write(address, 1);
             EEPROM.write(address2,44);
             gettextfromnode("","");
             eventhandler = true;
             change = false;
             break;
  
      }
}

void getnum2() {
  
  char key = customKeypad.waitForKey();

         switch (key) {
           case NO_KEY:
             // Wait and then tell user they can start the Serial Monitor and type in characters to
             break;
                               
            case '0':
              EEPROM.write(address3, 50);
              lcd.clear();
              change2 = false;
            break;
                   
            case '1':
              EEPROM.write(address3, 51);
              lcd.clear();
              change2 = false;
             break;

             case '2':
              EEPROM.write(address3, 52);
              lcd.clear();
              change2 = false;
             break;

             case '3':
              EEPROM.write(address3, 53);
              lcd.clear();
              change2 = false;
              break;

             case '4':
              kirimset = "504";
              lcd.clear();
              Serial2.println(kirimset);
              change2 = false;
             break;

              case '5':
              kirimset = "505";
              lcd.clear();
              Serial2.println(kirimset);
              change2 = false;
             break;

              case '6':
              kirimset = "506";
              lcd.clear();
              Serial2.println(kirimset);
              change2 = false;
             break;

              case '7':
              change2 = false;
              eventhandler = false;
              change3 = true;
              y = false;
              lcd.clear();
             break;

              case '8':
              change2 = false;
              EEPROM.put(0x0010, minus);
              j = 0;
              cursorColumn = 0;
              memset(minus, 0,  sizeof(minus));
              lcd.clear();
              change3 = false;
              resetFunc();
              eventhandler = true;
              y = true;
             break;

              case '9':
              lcd.clear();
              String jeko = EEPROM.get(0x0010, minustampil);
              jeko.replace(")","");
              gettextfromnode(jeko,"");
              change2 = false;
             break;
  
      }
}

void getnum3() {
  
  char key = customKeypad.waitForKey();
            if (key) {
              
              if (key == 'A') {
                key  = '-';
              }
              if(key == 'B'){
                key  = '+';
              }
              if(key == 'C'){
                key  = '.';
              }
              if(key == 'D'){
                key = "";
                lcd.clear();
                memset(minus, 0,  sizeof(minus));
                cursorColumn = 0;
              }else{
              lcd.setCursor(cursorColumn, 0);
              lcd.print(key);  
              cursorColumn++;

              minus[j] = key;
              j++;
              minus[j] = ')';
              j++;
              minus[j] = '\0';   
              }
              
              
                              
              if(cursorColumn == 16) {    
                lcd.clear();
                memset(minus, 0,  sizeof(minus));
                cursorColumn = 0;
              }
              
            if (key == "#")
            {
             
            }
        }     
    }
