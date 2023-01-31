#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
const char ssid[] = "coba";  // your network SSID (name)
const char pass[] = "123qweasd";   // your network password
WiFiClient  client;
int statusCode = 0;
const int RelayPin = D4;

int relayState = LOW;

//---------Channel Details---------//
unsigned long counterChannelNumber = 1981454;            // Channel ID
const char * myCounterReadAPIKey = "W23G81KHGN840240"; // Read API Key
const int FieldNumber1 = 1;  // The field you wish to read
//-------------------------------//

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin,  relayState);

}

void loop() {
  // put your main code here, to run repeatedly:
  //----------------- Network -----------------//
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println(" ....");
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("Connected to Wi-Fi Succesfully.");
  }
  //--------- End of Network connection--------//

  float kelembabanTanah = ThingSpeak.readLongField(counterChannelNumber, FieldNumber1, myCounterReadAPIKey);
  statusCode = ThingSpeak.getLastReadStatus();
  if (statusCode == 200)
  {
    if ((kelembabanTanah < 49)) {
      Serial.println(kelembabanTanah);
      relayState = LOW;
      digitalWrite(RelayPin, relayState);
    }
    else {
      relayState = HIGH;
      digitalWrite(RelayPin, relayState);
    }
  }
  else
  {
    Serial.println("Unable to read channel / No internet connection");
  }

}
