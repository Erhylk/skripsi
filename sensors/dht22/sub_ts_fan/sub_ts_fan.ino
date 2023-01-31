#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
const char ssid[] = "coba";  // your network SSID (name)
const char pass[] = "123qweasd";   // your network password
WiFiClient  client;
int statusCode = 0;

const int RelayPin = D4;

unsigned long lastTime = 0;
unsigned long timerDelay = 600000;



int relayState = LOW;
//---------Channel Details---------//
unsigned long counterChannelNumber = 1882138;            // Channel ID
const char * myCounterReadAPIKey = "SFSKXWLQ2HT8SQIJ"; // Read API Key
const int FieldNumber1 = 1;  // The field you wish to read
const int FieldNumber2 = 2;  // The field you wish to read
const int FieldNumber3 = 3;  // The field you wish to read
//-------------------------------//

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);

  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin,  relayState);
}

void loop()
{
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
  if ((millis() - lastTime) > timerDelay) {
    //---------------- Channel 1 ----------------//
    float humid = ThingSpeak.readLongField(counterChannelNumber, FieldNumber1, myCounterReadAPIKey);
    statusCode = ThingSpeak.getLastReadStatus();
    if (statusCode == 200)
    {
      Serial.print("humidity: ");
      Serial.println(humid);
    }
    else
    {
      Serial.println("Unable to read channel / No internet connection");
    }
    delay(100);
    //-------------- End of Channel 1 -------------//

    //---------------- Channel 2 ----------------//
    float temp = ThingSpeak.readLongField(counterChannelNumber, FieldNumber2, myCounterReadAPIKey);
    statusCode = ThingSpeak.getLastReadStatus();
    if (statusCode == 200)
    {
      Serial.print("temperature: ");
      Serial.println(temp);
    }
    else
    {
      Serial.println("Unable to read channel / No internet connection");
    }
    delay(100);
    //-------------- End of Channel 2 -------------//
    float output = ThingSpeak.readLongField(counterChannelNumber, FieldNumber3, myCounterReadAPIKey);
    statusCode = ThingSpeak.getLastReadStatus();
    if (statusCode == 200)
    {
      if ((output > 20)) {
        relayState = LOW;
        digitalWrite(RelayPin,  relayState );
        delay(output * 1000);
        Serial.print("output: ");
        Serial.println(output * 1000);
        relayState = HIGH;
        digitalWrite(RelayPin,  relayState );
      }
      else {
        relayState = HIGH;
        digitalWrite(RelayPin,  relayState );
      }

    }
    else
    {
      Serial.println("Unable to read channel / No internet connection");
    }
//    delay(15000);
    //-------------- End of Channel 3 -------------//

    lastTime = millis();
  }
}
