#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


unsigned long previousMillis = 0;
const long interval = 2000;

String arrData[1];

//untuk server
const char* serverName = "http://192.168.100.129:8080/ci4/public/admin/dashboard/setter";
 
const char* ssid = "RS232 TCP";       // Nama SSID AP/Hotspot
const char* password = "wifimesin@38";       // Password Wifi
 
// Mengatur IP Address ----------------------------------------------------
IPAddress IP(192,168,4,220);
IPAddress NETMASK(255,255,255,0);
IPAddress NETWORK(192,168,4,1);
IPAddress DNS(8,8,8,8);

String val = "";

void setup() {
  Serial.begin(9600);
 
// Connect to WiFi network ------------------------------------------------
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
// Mengatur WiFi ----------------------------------------------------------
  WiFi.mode(WIFI_STA);                      
  WiFi.begin(ssid, password);       
  WiFi.config(IP, NETWORK, NETMASK, DNS);
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
// Print status Connect ---------------------------------------------------
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
{

  if(WiFi.status()== WL_CONNECTED){

  WiFiClient client;
  HTTPClient http;
  
  previousMillis = currentMillis;


  String val = "";
  while (Serial.available() > 0)
  {
   val += char(Serial.read());  
  }
  val.trim();

 
  if(val != "")
  {

    int index =  0;

    for(int i=0; i<= val.length(); i++)
    {

       char delimiter = '#';
      if(val[i] != delimiter)
        arrData[index] += val[i];
      else
        index++; 
    }


    if (index == 0){

      
      http.begin(client, serverName);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
     
      
      String httpRequestData = "api_key=4321&timbangan=" + arrData[0];           

      int httpResponseCode = http.POST(httpRequestData);
 
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
     

      Serial.println("DATA TIMBANGAN              : " + arrData[0]); 
      Serial.println();
      
      http.end();
      
    }

    if (arrData[0] != ""){
      arrData[0] = "";
    } 
  
    
      }
    }else{
      Serial.println("WiFi Disconnected");
    }
  }
}
