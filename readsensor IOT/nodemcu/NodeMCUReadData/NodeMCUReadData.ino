#include <SoftwareSerial.h>
#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>

//buat variabel untuk software serial untuk mendefinisakn tx rx
//d6 dan d7 inisialisasi node mcu 12 dan 13
SoftwareSerial DataSerial(12, 13);

//millis sebagai pengganti delay
unsigned long previousMillis = 0;
const long interval = 3000;

//variable array untuk data parsing
String arrData[3];

//konfigurasi thinger IO
#define USERNAME "koper"
#define DEVICE_ID "Serial Thinger IO"
#define DEVICE_CREDENTIAL "kode"

//variable pin led apakah connect wifi atau tidak
#define LED_PIN 4 // pin 02 nodemcu

//variabel untuk thinger IO
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

//konifg wifi
const char* ssid = "nama wifi";
const char* password = "password wifi";

//menyediakan variable untuk menampung nilai sensor yang siap dikirim ke thinger IO
int ldr;
float temperature, humidity;

void setup() {
  Serial.begin(9600);
  DataSerial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  //koneksi ke wifi
  WiFi.begin(ssid, password);
  //cek koneksi
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    digitalWrite(LED_PIN, LOW);
  }
  //apabila terkoneksi
  digitalWrite(LED_PIN, HIGH);
  //hubugkan nodemcu ke thingerIO
  thing.add_wifi(ssid, password);

  //data yang akan dikrim
  thing["Dataku"] >> [](pson & out)
  {
    out["ldr"] = ldr;
    out["temperature"] = temperature;
    out["humidity"] = humidity;
  }
}

void loop() {
//konfigurasi millis
unsigned long currentMillis = millis(); //baca waktu millis saat ini
//waktu saat ini dikurangin waktu sebelumnya lebih dari interval atau 3 detik maka merefresh
if(currentMillis - previousMillis >= interval)
{
  //update previous millis
  previousMillis = currentMillis;

  //prioritaskan pembacaan dari arduino uno (hasil kiriman data)
  //baca data serial
  String data = "";
  while(DataSerial.available()>0)
  {
    data += char(DataSerial.read());
  }
  //buang spasi datanya
  data.trim();

  //uji data
  if(data != "")
  {
    //format data menggunakan pagar diparsing dlu
    int index =  0;
    //cek parsing data urutan
    for(int i=0; i<= data.length(); i++)
    {
      //pemisah untuk karakter pagar
      char delimiter = "#";
      if(data[i] != delimiter)
        arrData[index] += data[i];
      else
        index++; //variable bertambah 1
    }
    //pastikan data yang dikirim lengkap (ldr, temp, hum)
    //urutannya 0= LDR, 1 = temp, 2 = hum
    if (index == 2){
      //tampilkan nilai sensor ke serial monitor
      Serial.println("LDR              : " + arrData[0]); //LDR
      Serial.println("Temperatur       : " + arrData[1]); //temp
      Serial.println("Humidity         : " + arrData[2]); //Hum
      Serial.println();
    }
    //isi variable yang akan dikirim
    ldr = arrData[0].toInt();
    temperature = arrData[1].toFloat();
    humidity = arrData[2].toFLoat();

    //picu pengiriman data ke thinger IO
    thing.handle();
    
    //clear array data karena +=
    arrData[0] = "";
    arrData[1] = "";
    arrData[2] = ""; 
  }
  //minta data ke arduino uno
  DataSerial.println("KirimDataSekarang");
}

}
