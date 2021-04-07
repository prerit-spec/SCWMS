#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "FirebaseESP8266.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D5;
const int LOADCELL_SCK_PIN = D6;
const int blueLED = D8;
const int greenLED = D2;
const int whiteLED = D0;

HX711 scale;

// Set these to run example.
#define FIREBASE_HOST "*********.firebaseio.com"
#define FIREBASE_AUTH "*************************************"
#define WIFI_SSID "*******"
#define WIFI_PASSWORD "********"

//Define Firebase Data object
 FirebaseData firebaseData;
 FirebaseJson json;
 
const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String mytime;
int vr = A0; // variable resistor connected  
// GPIO where the DS18B20 is connected to
const int oneWireBus = 0;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup()
{
  // Debug console
  Serial.begin(9600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  sensors.begin();
  pinMode(vr ,INPUT);
  pinMode(blueLED,OUTPUT);
  pinMode(greenLED,OUTPUT);
  pinMode(whiteLED,OUTPUT);
  pinMode(D0,OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
      {
    digitalWrite(blueLED, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(blueLED, LOW);
    delay(250);
      }
      digitalWrite(greenLED, HIGH);
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseData.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  firebaseData.setResponseSize(1024);
}
 
void loop()
{
//timeClient.update();
//mytime = timeClient.getFormattedTime();
sensors.requestTemperatures(); 
float tempC = sensors.getTempCByIndex(0);
//serial printing the values for confirmation.
if (isnan(tempC)){
  Serial.println("Failed to read temprature data");
  delay(500);
  return;
}
Serial.print("Temperature (C) = ");
Serial.println(tempC);

if (scale.is_ready()) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    int weight = (reading*-1)/100;
    Serial.println(reading);
    Serial.println(weight);
    Firebase.setFloat(firebaseData, "users/FIREBASE_AUTH/Bed-1/level",weight);//LOADCELL
  } else {
    Serial.println("HX711 not found.");
  }



//uploading to firebase.
Firebase.setFloat(firebaseData, "users/FIREBASE_AUTH/Bed-1/temp",tempC);//TEMP
//Firebase.pushJSON(firebaseData, "users/FIREBASE_AUTH/esp/bed_03/temperature", json);

Serial.println("Data Pushed");

digitalWrite(whiteLED, HIGH);
delay(1000); 
digitalWrite(whiteLED, LOW);           
}
