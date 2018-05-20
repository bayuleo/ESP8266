
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
char ssid[] = "PUBG";    //  your network SSID (name)
char pass[] = "17081945";   // your network password
int status = WL_IDLE_STATUS;
WiFiClient  client;
unsigned long myChannelNumber = 479672;
const char * myWriteAPIKey = "5A8ZVNQJX3GY2OUE";
const char * myReadAPIKey = "U07D4LW8RAWN4JIIU07";

char val;
unsigned long sekarang, tadi;

const int sensorIn = A0;
int mVperAmp = 30; // use 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
float VRMS = 0;
float AmpsRMS = 0;
float Watt = 0;
float wattm = 0;
float wattmenit = 0;
float wattjam = 0;
float dump1 = 0;
float dump2 = 0;
float data_baca;
int detik = 0;
int menit = 0;
int jam = 0;
int kirim = 0;
int baca = 0;

unsigned long time_awal_esp, time_sekarang_esp;
unsigned long time_awal, time_sekarang;
unsigned long xxxx, yyyy;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  ThingSpeak.begin(client);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  sekarang = millis();
  tadi = millis();
}

void loop() {
  countwatt();
  relay();
  if (kirim == 1) {
    int sensorValue = random(1000);
    ThingSpeak.writeField(myChannelNumber, 3, dump2, myWriteAPIKey);
    Serial.println("KIRIM");
    kirim = 0;
  }
  if (baca == 1) {
    data_baca = ThingSpeak.readFloatField(myChannelNumber, 1, myReadAPIKey);
    Serial.println("BACA");
    baca = 0;
  }
  //delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}

void relay() {
  if ( Serial.available() > 0 ) {
    val = Serial.read();
    Serial.println(val);
  }
  if ( data_baca > 0 ) {
    digitalWrite(2, HIGH);
  } //Saklar Mati
  else if ( data_baca < 1 ) {
    digitalWrite(2, LOW);
  } //Saklar Hidup
  sekarang = millis();
  if (sekarang - tadi > 1000) {
    Serial.print("Nilai Val = ");
    Serial.println(val);
    Serial.print("Nilai Kirim = ");
    Serial.println(kirim);
    Serial.print("Nilai Baca = ");
    Serial.println(data_baca);
    tadi = millis();
    detik++;
  }

}

float getVPP() {
  float result;

  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(sensorIn);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the maximum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 5.0) / 1024.0;

  return result;
}

void countwatt() {

  Voltage = getVPP();
  VRMS = (Voltage / 2.0) * 0.707;
  AmpsRMS = ((VRMS * 1000) / mVperAmp) - 0.1;
  Watt = AmpsRMS * Voltage * 1000;
  Serial.print(AmpsRMS);
  Serial.print(" Amps RMS || ");
  Serial.print(VRMS);
  Serial.print(" VRMS || ");
  Serial.print(Voltage);
  Serial.print(" V || ");
  Serial.print(Watt);
  Serial.println(" Watt");

  dump1 = dump1 + Watt;

  //detik = detik + 1;
  if ( detik > 59) {
    detik = 0;
    menit = menit + 1;
    dump2 = dump1 / 3600;
    dump1 = 0;
    kirim = 1;
    baca = 1;
  }
  if (menit > 59) {
    menit = 0;
    jam = jam + 1;
  }
  Serial.print("Time : ");
  Serial.print(jam);
  Serial.print(":");
  Serial.print(menit);
  Serial.print(":");
  Serial.println(detik);

  wattm = (wattm + Watt);
  wattmenit = wattm / 60;
  wattjam = wattm / 3600;
  //Serial.print("Watt/Menit = ");
  //Serial.println(wattmenit);
  //Serial.print("Watt/Jam = ");
  //Serial.println(wattjam);
  //Serial.print("Dump1 = ");
  //Serial.println(dump1);
  Serial.print("Dump2 = ");
  Serial.println(dump2);
}
