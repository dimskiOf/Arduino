const char* ssid = "warriornux.com";      // Nama AP/Hotspot
const char* password = "1234567890";    // Password AP/Hotspot
 
// Mengatur IP Address ----------------------------------------------------
IPAddress IP(192,168,5,1);
IPAddress NETMASK(255,255,255,0);
IPAddress NETWORK(192,168,5,1);
IPAddress DNS(192,168,5,1);
  
WiFiServer server(80);
void setup() {
  Serial.begin(115200);
  delay(10);
  
 // Mengatur WiFi ----------------------------------------------------------
  Serial.println();
  Serial.print("Configuring access point...");
   
  WiFi.mode(WIFI_AP);             // Mode AP/Hotspot
  WiFi.softAP(ssid, password);
  WiFi.config(IP, NETWORK, NETMASK, DNS);
  
 // Start the server -------------------------------------------------------
  server.begin();
  Serial.println("Server dijalankan");
  
 // Print the IP address ---------------------------------------------------
  Serial.println(WiFi.localIP());
 }
 
void loop() {
  // put your main code here, to run repeatedly:
 
}
