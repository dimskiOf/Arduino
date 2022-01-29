#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>

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
  Serial.println("");
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;

void setup() {
  Serial.begin( 9600 );

  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );

  Hid.SetReportParser(0, &Parser);
}

void loop() {
  Usb.Task();
  if (x){
    Serial.print(DataBarcode);
    x = false;
    DataBarcode = "";
  }
} 
