#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#define trigger D0
#define LED D2
#define power D3

const unsigned char Active_buzzer = 14;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode (Active_buzzer,OUTPUT);
    
    pinMode(trigger,INPUT); 
    pinMode(LED,OUTPUT); 
     pinMode(power,OUTPUT);    
    if(digitalRead(trigger) == HIGH){
    digitalWrite(power,HIGH);
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    wifiManager.autoConnect("CIRCUIT DIGEST WiFi Manager");
    Serial.println("connected :)");
  }     
}
void loop() { 
  if (WiFi.status() == WL_CONNECTED)
        {
          digitalWrite(power,LOW);
          while(WiFi.status() == WL_CONNECTED){           
            suarastart();   
          }              
        }
        else {
          digitalWrite(LED,LOW);
        }
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
