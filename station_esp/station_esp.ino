#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <string.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   

SoftwareSerial NodeMCU(D1, D2);
SoftwareSerial NodeMCUTimbangan(D3, D4);

unsigned long previousMillis = 0;
const long interval = 0;
const long intervals = 3000;

String arrData[1];
String arrData2[1];

#define EEPROM_SIZE 12

//inisialisasi tone
int length = 15; // the number of notes

//twinkle twinkle little star
char notes[] = "ccggaag ffeeddc ggffeed ggffeed ccggaag ffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;

//timbangandanbarcodermkeluar
const char* serverRmkeluar = "http://192.168.100.129/ci3/operator99/inputdataarduinorm99keluar";
const char* serverRmmasuk = "http://192.168.100.129/ci3/operator99/inputdataarduinorm99masuk";
const char* serverFgkeluar = "http://192.168.100.129/ci3/operator99/inputdataarduinofg99keluar";
const char* serverFgmasuk = "http://192.168.100.129/ci3/operator99/inputdataarduinofg99masuk";
 
String val = "";
String val2 = "";
String val3 = "";
String val4 = "";
String tampung = "";
float reading;

const unsigned char Active_buzzer = 14;

int suara = 0;

int address = 0;
int address2 = 1;

void setup() {
  Serial.begin(19200);
  NodeMCU.begin(19200);
  NodeMCUTimbangan.begin(9600);
  pinMode (Active_buzzer,OUTPUT) ;
// suara indikator hidup
suarastart();

//init eeprom
EEPROM.begin(EEPROM_SIZE);

 if(EEPROM.read(address2) == 100){
    EEPROM.write(address2,0);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    wifiManager.autoConnect("Setting Wifi");
    Serial.println("connected :)");
    indikatordisplay("Setting");
  }      
}

void loop() {       
while (NodeMCU.available() > 0)
  {
   tampung += char(NodeMCU.read());
   tampung.trim();  
  }
 delay(100);
if (tampung == "500"){
EEPROM.write(address, 500);
EEPROM.commit();
tampung = "";
indikatorgantimode(); 
}else if(tampung == "501"){
EEPROM.write(address, 501);
EEPROM.commit();
tampung = "";
indikatorgantimode(); 
}else if(tampung == "502"){
EEPROM.write(address, 502);
EEPROM.commit();
tampung = "";
indikatorgantimode();   
}else if(tampung == "503"){
EEPROM.write(address, 503);
EEPROM.commit();
tampung = "";
indikatorgantimode(); 
}else if(tampung == "504"){
EEPROM.write(address2, 100);
EEPROM.commit();
tampung = "";
}

if(tampung != ""){
  //Serial.println(tampung);
  tampung = "";
}

reading = EEPROM.read(address);

if(reading == 244.00){
  reading = 0.00;
  inputrmgd99keluar();
}else if(reading == 245.00){
  reading = 0.00;
  inputrmgd99masuk();
}else if(reading == 246.00){
  reading = 0.00;
  inputfggd99keluar();
}else if(reading == 247.00){
  reading = 0.00;
  inputfggd99masuk();
}

}

void inputrmgd99keluar(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
{
  if(WiFi.status()== WL_CONNECTED){
  indikatorconnectwifi();
  
  WiFiClient client;
  HTTPClient http;
  
  previousMillis = currentMillis;
  while (Serial.available() > 0)
  {
   val += char(Serial.read());
   val.replace("$*","");
   val.trim();  
  }
  if(val != "")
  {    
      http.begin(client, serverRmkeluar);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if ((httpResponseCode == -1) or (httpResponseCode < 1)){
        indikatordisplay("Servier Die");
      
      }else{
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

      auto error = deserializeJson(doc, payload);
        if (error) {
            indikatordisplay(error.c_str());
            return;
        }

      String value = doc["pesan"];
      indikatordisplay(value);
      
         
      Serial.println("HTTP Response code: ");
      Serial.println(httpResponseCode);

      value = "";
      
      http.end();

      if (val != ""){
        val = "";
      }
      
      }
    }
    }else{
      indikatordisplay("disconnect");
      }
  }
}

void inputrmgd99masuk(){
unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
{
  if(WiFi.status()== WL_CONNECTED){
  indikatorconnectwifi();
  
  WiFiClient client;
  HTTPClient http;
  
  previousMillis = currentMillis;
  while (Serial.available() > 0)
  {
   val += char(Serial.read());
   val.replace("$*","");
   val.trim();  
  }
  if(val != "")
  {    
      http.begin(client, serverRmmasuk);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if ((httpResponseCode == -1) or (httpResponseCode < 1)){
        indikatordisplay("Servier Die");
      
      }else{
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

      auto error = deserializeJson(doc, payload);
        if (error) {
            indikatordisplay(error.c_str());
            return;
        }

      String value = doc["pesan"];
      indikatordisplay(value);
      
         
      Serial.println("HTTP Response code: ");
      Serial.println(httpResponseCode);

      value = "";
      
      http.end();

      if (val != ""){
        val = "";
      }
      
      }
    }
    }else{
      indikatordisplay("disconnect");
      }
  }
}

void inputfggd99keluar(){
 unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
{
  if(WiFi.status()== WL_CONNECTED){
  indikatorconnectwifi();
  
  WiFiClient client;
  HTTPClient http;
  
  previousMillis = currentMillis;
  while (Serial.available() > 0)
  {
   val += char(Serial.read());
   val.replace("$*","");
   val.trim();  
  }
  if(val != "")
  {    
      http.begin(client, serverFgkeluar);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if ((httpResponseCode == -1) or (httpResponseCode < 1)){
        indikatordisplay("Servier Die");
      
      }else{
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

      auto error = deserializeJson(doc, payload);
        if (error) {
            indikatordisplay(error.c_str());
            return;
        }

      String value = doc["pesan"];
      indikatordisplay(value);
      
         
      Serial.println("HTTP Response code: ");
      Serial.println(httpResponseCode);

      value = "";
      
      http.end();

      if (val != ""){
        val = "";
      }
      
      }
    }
    }else{
      indikatordisplay("disconnect");
      }
  }
}

void inputfggd99masuk(){
   unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
{
  if(WiFi.status()== WL_CONNECTED){
  indikatorconnectwifi();
  
  WiFiClient client;
  HTTPClient http;
  
  previousMillis = currentMillis;
  while (Serial.available() > 0)
  {
   val += char(Serial.read());
   val.replace("$*","");
   val.trim();  
  }
  if(val != "")
  {    
      http.begin(client, serverFgmasuk);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if ((httpResponseCode == -1) or (httpResponseCode < 1)){
        indikatordisplay("Servier Die");
      
      }else{
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

      auto error = deserializeJson(doc, payload);
        if (error) {
            indikatordisplay(error.c_str());
            return;
        }

      String value = doc["pesan"];
      indikatordisplay(value);
      
         
      Serial.println("HTTP Response code: ");
      Serial.println(httpResponseCode);

      value = "";
      
      http.end();

      if (val != ""){
        val = "";
      }
      
      }
    }
    }else{
      indikatordisplay("disconnect");
      }
  }
}

void indikatordisplay(String indik){
  NodeMCU.println(indik);
}

void indikatordisplay2(String indik){
  NodeMCU.print(indik);
}

void indikatorconnectwifi(){
  digitalWrite(BUILTIN_LED, LOW);
  delay(1000);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(1000);
}

void indikatorgantimode(){
    tone(Active_buzzer, 2000) ;
    delay (1000); 
    tone(Active_buzzer, 2000) ;
    delay (1000); 
    noTone(Active_buzzer) ;
}

void indikatorkirimdata(){
  digitalWrite(BUILTIN_LED, LOW);
  delay(300);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(300);
  digitalWrite(BUILTIN_LED, LOW);
  delay(300);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(300);
  digitalWrite(BUILTIN_LED, LOW);
  delay(300);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(300);
}

void indikatorkirimdata2(){
  digitalWrite(BUILTIN_LED, LOW);
  delay(3000);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(3000);
  digitalWrite(BUILTIN_LED, LOW);
  delay(3000);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(3000);
  digitalWrite(BUILTIN_LED, LOW);
  delay(3000);
  digitalWrite(BUILTIN_LED, HIGH);
  delay(3000);
}

void suarastart(){
  tone(Active_buzzer, 1000) ; 
    delay (100); 
    tone(Active_buzzer, 2000) ; 
    delay (100); 
    tone(Active_buzzer, 3000) ; 
    delay (100); 
    tone(Active_buzzer, 4000) ; 
    delay (100); 
    tone(Active_buzzer, 5000) ;
    delay (100); 
    tone(Active_buzzer, 6000) ;
    delay (100); 
    tone(Active_buzzer, 7000) ; 
    delay (100); 
    noTone(Active_buzzer) ; 
}

void inputsuara(){
   
    tone(Active_buzzer, 2000) ;
    delay (500); 
    tone(Active_buzzer, 4000) ; 
    delay (500); 
    noTone(Active_buzzer) ;
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(Active_buzzer, HIGH);
    delayMicroseconds(tone);
    digitalWrite(Active_buzzer, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  
  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void twinkle2littlestar(){
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    
    // pause between notes
    delay(tempo / 2); 
  }
}
