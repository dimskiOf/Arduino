void setup() {
Serial.begin(9600);
}

void loop() {
if(Serial.available())
{
  int counts = 0;
 while(Serial.read())
 {
  char input = Serial.read();
  Serial.print(input);
  counts++;
  delay(5); 
 }
 Serial.println();
}
}
