#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#define SensorPin A0

const char* ssid = "coba";   // your network SSID (name)
const char* password = "123qweasd";   // your network password

WiFiClient  client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "UHRI43B4JPX5CCE5";

void setup() {

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  Serial.begin(115200);

}

void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  float sensorValue = analogRead(SensorPin);
  Serial.println(sensorValue);

  float kelembabanTanah;
  int hasilPembacaan = analogRead(SensorPin);
  kelembabanTanah = (100 - ((hasilPembacaan / 1023.00) * 100));
  Serial.print(kelembabanTanah);
  Serial.print("%");
  delay(10000);

  ThingSpeak.setField(1, kelembabanTanah);

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
}
