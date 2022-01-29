//sensor DHT
#include <DHT.h>

//konfig sensor DHT11
#define DHT_PIN 8
#define DHTTYPE DHT11

//variabel untuk sensor DHT
DHT dht(DHT_PIN,DHTTYPE);

//variabel menampung nilai sensor kelembaban dan temperatur
int ldr;
float humadity, temperature;

void setup() {
  Serial.begin(9600);
  //aktifkan sensor DHT
  dht.begin();

}

void loop() {
  //baca permintaan dari nodeMCU
  String minta = "";
  //baca permintaan nodemcu
  while(Serial.available()>0){
    minta += char(Serial.read());
  }

  //buang spasi data yang diterima
  minta.trim();
  //uji variabel minta
  if(minta == "KirimDataSekarang"){
    //kirim datanya
    kirimdata();
  }
  //clear data minta
  minta = "";
  //arduino cek setiap 1 detik apakah ada permintaan
  delay(1000);
}

void kirimdata()
{
  //baca sensor LDR pin A0
  ldr = analogRead(0);
  //baca temperatur dan humadity
  temperature = dht.readTemperature();
  humidity    = dht.readHumidity();

  //variabel penampung data yang akan dikrim
  //format data kirim dalam format ldr#temperature#humidity
  String datakirim = String(ldr) + "#" + String(temperature) + "#" + String(humidity);
  //kirim data ke nodemcu
  Serial.println(datakirim);
}
