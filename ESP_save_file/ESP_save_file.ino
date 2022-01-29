#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <string.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>   
#include <Hash.h>
#include <FS.h>

ESP8266WebServer server(8080);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleInput();
void handleInput2();
void handleNotFound();
void handleSeturl();
void handleSetip();

int address2 = 1;

SoftwareSerial NodeMCU(D1, D2);
SoftwareSerial NodeMCUTimbangan(D3, D4);

unsigned long previousMillis = 0;
const long interval = 0;
const long intervals = 3000;

String arrData[1];
String arrData2[1];



//inisialisasi tone
int length = 15; // the number of notes

//twinkle twinkle little star
char notes[] = "ccggaag ffeeddc ggffeed ggffeed ccggaag ffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int tempo = 300;
 
String val = "";
String val2 = "";
String val3 = "";
String val4 = "";
String tampung = "";
float reading;

const unsigned char Active_buzzer = 14;

int suara = 0;

int address = 0;

#define EEPROM_SIZE 12

String readFile(fs::FS &fs, const char * path){
 // Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
   // Serial.println("- empty file or failed to open file");
    return String();
  }
 // Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  file.close();
  //Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
 // Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
   // Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
   // Serial.println("- file written");
  } else {
   // Serial.println("- write failed");
  }
  file.close();
}



void setup(void) {
   Serial.begin(19200);
    NodeMCU.begin(19200);
    NodeMCUTimbangan.begin(9600);
    pinMode (Active_buzzer,OUTPUT) ;
  // suara indikator hidup
  suarastart();
  

   //init eeprom
    EEPROM.begin(EEPROM_SIZE);
    SPIFFS.begin();
    
   if(EEPROM.read(address2) == 100){
    SPIFFS.remove("/inputString5.txt");
    SPIFFS.remove("/inputString6.txt");
    SPIFFS.remove("/inputString7.txt");
    SPIFFS.remove("/inputString8.txt");

    EEPROM.write(address2,0);
    EEPROM.commit();
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    wifiManager.autoConnect("Setting Wifi");
    indikatordisplay("Setting");
  }

  WiFiManager wifiManager;
  
  if (readFile(SPIFFS, "/inputString5.txt") == ""){
  
  }else{
  String static_ip = readFile(SPIFFS, "/inputString5.txt");
  String static_gw = readFile(SPIFFS, "/inputString6.txt");
  String static_sn = readFile(SPIFFS, "/inputString7.txt");
  String static_dns = readFile(SPIFFS, "/inputString8.txt");

  IPAddress _ip, _gw, _sn, _dns;
  _ip.fromString(static_ip);
  _gw.fromString(static_gw);
  _sn.fromString(static_sn);
  _dns.fromString(static_dns);

  wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn, _dns);
  }
  
  if (!wifiManager.autoConnect("Setting Wifi")) {
    SPIFFS.begin();
    SPIFFS.remove("/inputString5.txt");
    SPIFFS.remove("/inputString6.txt");
    SPIFFS.remove("/inputString7.txt");
    SPIFFS.remove("/inputString8.txt");

    EEPROM.write(address2,0);
    EEPROM.commit();
    wifiManager.resetSettings();
    indikatordisplay("Setting");
  }
  
  // Initialize SPIFFS
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }

   if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", HTTP_GET, handleRoot);        // Call the 'handleRoot' function when a client requests URI "/"
  server.on("/seturl", HTTP_GET, handleSeturl);
  server.on("/setip", HTTP_GET, handleSetip);
  server.on("/set1", HTTP_POST, handleInput);
  server.on("/set2", HTTP_POST, handleInput2);// Call the 'handleLogin' function when a POST request is made to URI "/login"
  server.onNotFound(handleNotFound);           // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
 
}

void loop(void) {
 server.handleClient();

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
}else if(tampung == "505"){
ESP.restart();
tampung = "";
}else if(tampung == "506"){
indikatordisplay2();
tampung = "";
}else if(tampung == "507"){
EEPROM.write(address, 504);
EEPROM.commit();
tampung = "";
indikatorgantimode();   
}

if(tampung != ""){
  //Serial.println(tampung);
  tampung = "";
}

reading = EEPROM.read(address);

if(reading == 244.00){
  //timbangandanbarcodermkeluar
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
}else if(reading == 248.00){
  reading = 0.00;
  barangsisa();
}
 
}

void handleRoot() {         
  server.send(200, "text/html", "<!DOCTYPE html><html><head><style>.button {background-color: #4CAF50;border: none;color: white; padding: 15px 32px;text-align: center;text-decoration: none;display: inline-block;font-size: 16px;margin: 4px 2px;cursor: pointer;} </style> </head> <body> <center><h1>Silahkan Setting Url atau Ip</h1></center> <center><a href=\"/seturl\" class=\"button\">Setting Url</a></center><br> <center><a href=\"/setip\" class=\"button\">Setting Ip</a></center> </body> </html>");

  }

void handleSetip(){
  server.send(200,"text/html","<!DOCTYPE html><html><title>Setting Ip</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\"><body><div class=\"w3-card-4\"><div class=\"w3-container w3-brown\"><h2>Input Setting Ip</h2></div><form class=\"w3-container\" action=\"/set2\" method=\"POST\"><p><label class=\"w3-text-brown\"><b>Ip</b></label><input class=\"w3-input w3-border w3-sand\" name=\"ip\" type=\"text\" value=\""+ readFile(SPIFFS, "/inputString5.txt") +"\"></p><p><label class=\"w3-text-brown\"><b>Gateway</b></label><input class=\"w3-input w3-border w3-sand\" name=\"gateway\" type=\"text\" value=\""+ readFile(SPIFFS, "/inputString6.txt") +"\"></p><p><label class=\"w3-text-brown\"><b>Subnet</b></label><input class=\"w3-input w3-border w3-sand\" name=\"subnet\" type=\"text\" value=\""+ readFile(SPIFFS, "/inputString7.txt") +"\"></p><p><label class=\"w3-text-brown\"><b>Dns</b></label><input class=\"w3-input w3-border w3-sand\" name=\"dns\" type=\"text\" value=\""+ readFile(SPIFFS, "/inputString8.txt") +"\"></p><p><button class=\"w3-btn w3-brown\">Submit</button></p></form></div></body></html> ");
}



void handleSeturl(){
  server.send(200, "text/html", "<!DOCTYPE html><html><title>Setting Url</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\"><body><div class=\"w3-card-4\"><div class=\"w3-container w3-brown\"><h2>Input Setting Ip</h2></div><form class=\"w3-container\" action=\"/set1\" method=\"POST\"><label class=\"w3-text-brown\"><b>Server RM Keluar</b></label><input class=\"w3-input w3-border w3-sand\" type=\"text\" name=\"rmkeluar\" placeholder=\"serverrmkeluar\" value=\""+ readFile(SPIFFS, "/inputString.txt") +"\"></p><label class=\"w3-text-brown\"><b>Server RM Masuk</b></label><input class=\"w3-input w3-border w3-sand\" type=\"text\" name=\"rmmasuk\" placeholder=\"serverrmmasuk\" value=\""+ readFile(SPIFFS, "/inputString2.txt") +"\"></p><label class=\"w3-text-brown\"><b>Server FG Keluar</b></label><input class=\"w3-input w3-border w3-sand\" type=\"text\" name=\"fgkeluar\" placeholder=\"serverfgkeluar\" value=\""+ readFile(SPIFFS, "/inputString3.txt") +"\"></p><label class=\"w3-text-brown\"><b>Server FG Masuk</b></label><input class=\"w3-input w3-border w3-sand\" type=\"text\" name=\"fgmasuk\" placeholder=\"serverfgmasuk\" value=\""+ readFile(SPIFFS, "/inputString4.txt") +"\"></p><label class=\"w3-text-brown\"><b>Server Barang Sisa</b></label><input class=\"w3-input w3-border w3-sand\" type=\"text\" name=\"barangsisa\" placeholder=\"barang sisa\" value=\""+ readFile(SPIFFS, "/inputString9.txt") +"\"></p><p><button class=\"w3-btn w3-brown\">Submit</button></p></form></div></body></html>");
}

void handleInput() {  
  //rmkeluar  
  if( ! server.hasArg("rmkeluar") || server.arg("rmkeluar") == NULL || ! server.hasArg("rmmasuk") || server.arg("rmmasuk") == NULL ||  ! server.hasArg("fgkeluar") || server.arg("fgkeluar") == NULL || ! server.hasArg("fgmasuk") || server.arg("fgmasuk") == NULL  || ! server.hasArg("barangsisa") || server.arg("barangsisa") == NULL ) {
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }else{
    String inputMessage;
    //rmkeluar
    inputMessage = server.arg("rmkeluar");
    writeFile(SPIFFS, "/inputString.txt", inputMessage.c_str());
    String yourInputString = readFile(SPIFFS, "/inputString.txt");

    String inputMessage2;
    //rmmasuk
    inputMessage2 = server.arg("rmmasuk");
    writeFile(SPIFFS, "/inputString2.txt", inputMessage2.c_str());
    String yourInputString1 = readFile(SPIFFS, "/inputString2.txt");
 
    String inputMessage3;
    //rmkeluar
    inputMessage3 = server.arg("fgkeluar");
    writeFile(SPIFFS, "/inputString3.txt", inputMessage3.c_str());
    String yourInputString3 = readFile(SPIFFS, "/inputString3.txt");

 
    String inputMessage4;
    //rmkeluar
    inputMessage4 = server.arg("fgmasuk");
    writeFile(SPIFFS, "/inputString4.txt", inputMessage4.c_str());
    String yourInputString4 = readFile(SPIFFS, "/inputString4.txt");

    String inputMessage9;
    //barangsisa
    inputMessage9 = server.arg("barangsisa");
    writeFile(SPIFFS, "/inputString9.txt", inputMessage9.c_str());
    String yourInputString9 = readFile(SPIFFS, "/inputString9.txt");
   
    server.send(200, "text/html", "<p>success input data</p>" +yourInputString +yourInputString1 +yourInputString3 +yourInputString4 +yourInputString9 + "<br><button onclick=\"window.history.go(-1); return false;\" class=\"w3-btn w3-brown\">Back</button> ");
    ESP.restart();
  }
}

void handleInput2() {  
    String inputMessage5;
    //ip
    inputMessage5 = server.arg("ip");
    writeFile(SPIFFS, "/inputString5.txt", inputMessage5.c_str());
    String yourInputString5 = readFile(SPIFFS, "/inputString5.txt");

    String inputMessage6;
    //gateway
    inputMessage6 = server.arg("gateway");
    writeFile(SPIFFS, "/inputString6.txt", inputMessage6.c_str());
    String yourInputString6 = readFile(SPIFFS, "/inputString6.txt");
 
    String inputMessage7;
    //subnet
    inputMessage7 = server.arg("subnet");
    writeFile(SPIFFS, "/inputString7.txt", inputMessage7.c_str());
    String yourInputString7 = readFile(SPIFFS, "/inputString7.txt");

 
    String inputMessage8;
    //dns
    inputMessage8 = server.arg("dns");
    writeFile(SPIFFS, "/inputString8.txt", inputMessage8.c_str());
    String yourInputString8 = readFile(SPIFFS, "/inputString8.txt");
   
    server.send(200, "text/html", "<p>success input data</p>" +yourInputString5 +yourInputString6 +yourInputString7 +yourInputString8 + "<br><button onclick=\"window.history.go(-1); return false;\" class=\"w3-btn w3-brown\">Back</button> ");
    ESP.restart();
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
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
      String serverRmkeluar = readFile(SPIFFS, "/inputString.txt"); 
      http.begin(client, serverRmkeluar);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
       
      String payload = http.getString(); //payload json
       
      if (httpResponseCode > 0){
      DynamicJsonDocument doc(1024);
      
      indikatorkirimdata();
      inputsuara();
      

      DeserializationError error = deserializeJson(doc, payload);

       if (error) {
        Serial.print(F("deserializeJson() failed: "));
        indikatordisplay(error.f_str());
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
      String serverRmmasuk = readFile(SPIFFS, "/inputString2.txt"); 
      http.begin(client, serverRmmasuk);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if (httpResponseCode > 0){
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

       DeserializationError error = deserializeJson(doc, payload);

       if (error) {
        Serial.print(F("deserializeJson() failed: "));
        indikatordisplay(error.f_str());
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
      String serverFgkeluar = readFile(SPIFFS, "/inputString3.txt"); 
      http.begin(client, serverFgkeluar);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if (httpResponseCode > 0){
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

       DeserializationError error = deserializeJson(doc, payload);

       if (error) {
        Serial.print(F("deserializeJson() failed: "));
        indikatordisplay(error.f_str());
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
      String serverFgmasuk = readFile(SPIFFS, "/inputString4.txt"); 
      http.begin(client, serverFgmasuk);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if (httpResponseCode > 0){
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

       DeserializationError error = deserializeJson(doc, payload);

       if (error) {
        Serial.print(F("deserializeJson() failed: "));
        indikatordisplay(error.f_str());
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

void barangsisa(){
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
      String serverBarangsisa = readFile(SPIFFS, "/inputString9.txt"); 
      http.begin(client, serverBarangsisa);


      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
      
      String httpRequestData = "api_key=4321&timbangandanbarcode=" + val;           

      int httpResponseCode = http.POST(httpRequestData);
      String payload = http.getString(); //payload json

      if (httpResponseCode > 0){
     
      indikatorkirimdata();
      inputsuara();
      DynamicJsonDocument doc(1024);

       DeserializationError error = deserializeJson(doc, payload);

       if (error) {
        Serial.print(F("deserializeJson() failed: "));
        indikatordisplay(error.f_str());
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

void indikatordisplay2(){
  NodeMCU.print(WiFi.localIP());
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
