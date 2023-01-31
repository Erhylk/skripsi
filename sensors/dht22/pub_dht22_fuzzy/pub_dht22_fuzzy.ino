#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <DHT.h>
#include <DHT_U.h>
#include <Fuzzy.h>

#define DHTPIN D3      //d2 pin
#define DHTTYPE DHT22 // it was DHT22

const char* ssid = "coba";   // your network SSID (name)
const char* password = "123qweasd";   // your network password

WiFiClient  client;

DHT dht(DHTPIN, DHTTYPE);

unsigned long myChannelNumber = 3;
const char * myWriteAPIKey = "WX7KZLL7MJ6TDROY";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 180000;

float humid;
float temp;
//int sensorPin = A0;

//Fuzzy
Fuzzy *fuzzy = new Fuzzy();

//FuzzyInput - Humidity
FuzzySet *kering = new FuzzySet(20, 30, 40, 50);
FuzzySet *medium = new FuzzySet(30, 50, 70, 90);
FuzzySet *basah = new FuzzySet(70, 80, 90, 100);

//FuzzyInput - Temperature
FuzzySet *dingin = new FuzzySet(5, 10, 15, 20);
FuzzySet *normal = new FuzzySet(15, 20, 25, 30);
FuzzySet *panas = new FuzzySet(25, 30, 35, 40);

//FuzzyOutput - fan
FuzzySet *mati = new FuzzySet(0, 0, 0, 0);
FuzzySet *sebentar = new FuzzySet(0, 0, 20, 40);
FuzzySet *lama = new FuzzySet(20, 40, 60, 80);

void setup()
{

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  // Set the Serial output
  Serial.begin(115200);
  // Set a random seed
  randomSeed(analogRead(0));
  Serial.print("Data from Sensors \n");
  pinMode(DHTPIN, INPUT); //DHT sensor
  dht.begin();

  //FuzzyInput
  FuzzyInput *humid = new FuzzyInput(1);

  humid->addFuzzySet(kering);
  humid->addFuzzySet(medium);
  humid->addFuzzySet(basah);
  fuzzy->addFuzzyInput(humid);

  //FuzzyInput
  FuzzyInput *temp = new FuzzyInput(2);

  temp->addFuzzySet(dingin);
  temp->addFuzzySet(normal);
  temp->addFuzzySet(panas);
  fuzzy->addFuzzyInput(temp);

  //FuzzyOutput
  FuzzyOutput *fan = new FuzzyOutput(1);
  fan->addFuzzySet(mati);
  fan->addFuzzySet(sebentar);
  fan->addFuzzySet(lama);
  fuzzy->addFuzzyOutput(fan);

  //Building FuzzyRule - 1 (Kelembaban kering & Suhu dingin - kipas mati)
  FuzzyRuleAntecedent *hkeringAndtdingin = new FuzzyRuleAntecedent();
  hkeringAndtdingin->joinWithAND(kering, dingin);
  FuzzyRuleConsequent *thenFanMati = new FuzzyRuleConsequent();
  thenFanMati->addOutput(mati);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, hkeringAndtdingin, thenFanMati);
  fuzzy->addFuzzyRule(fuzzyRule1);

  //Building FuzzyRule - 2 (Kelembaban kering & Suhu normal - kipas sebentar)
  FuzzyRuleAntecedent *hkeringAndtnormal = new FuzzyRuleAntecedent();
  hkeringAndtnormal->joinWithAND(kering, normal);
  FuzzyRuleConsequent *thenFanSebentar = new FuzzyRuleConsequent();
  thenFanSebentar->addOutput(sebentar);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, hkeringAndtnormal, thenFanSebentar);
  fuzzy->addFuzzyRule(fuzzyRule2);

  //Building FuzzyRule - 3 (Kelembaban kering & Suhu panas - kipas lama)
  FuzzyRuleAntecedent *hkeringAndtpanas = new FuzzyRuleAntecedent();
  hkeringAndtpanas->joinWithAND(kering, panas);
  FuzzyRuleConsequent *thenFanLama = new FuzzyRuleConsequent();
  thenFanLama->addOutput(lama);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, hkeringAndtpanas, thenFanLama);
  fuzzy->addFuzzyRule(fuzzyRule3);

  //Building FuzzyRule - 4 (Kelembaban medium & suhu dingin - kipas mati)
  FuzzyRuleAntecedent *hmediumAndtdingin = new FuzzyRuleAntecedent();
  hmediumAndtdingin->joinWithAND(medium, dingin);
  FuzzyRuleConsequent *thenFanMati4 = new FuzzyRuleConsequent();
  thenFanMati4->addOutput(mati);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, hmediumAndtdingin, thenFanMati4);
  fuzzy->addFuzzyRule(fuzzyRule4);

  //Building FuzzyRule - 5 (Kelembaban medium & suhu normal - kipas mati)
  FuzzyRuleAntecedent *hmediumAndtnormal = new FuzzyRuleAntecedent();
  hmediumAndtnormal->joinWithAND(medium, normal);
  FuzzyRuleConsequent *thenFanMati5 = new FuzzyRuleConsequent();
  thenFanMati5->addOutput(mati);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, hmediumAndtnormal, thenFanMati5);
  fuzzy->addFuzzyRule(fuzzyRule5);

  //Building FuzzyRule - 6 (Kelembaban medium & suhu panas - kipas lama)
  FuzzyRuleAntecedent *hmediumAndtpanas = new FuzzyRuleAntecedent();
  hmediumAndtpanas->joinWithAND(medium, panas);
  FuzzyRuleConsequent *thenFanLama6 = new FuzzyRuleConsequent();
  thenFanLama6->addOutput(lama);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(5, hmediumAndtpanas, thenFanLama6);
  fuzzy->addFuzzyRule(fuzzyRule6);

  //Building FuzzyRule - 7 (Kelembaban basah & suhu dingin - kipas mati)
  FuzzyRuleAntecedent *hbasahAndtdingin = new FuzzyRuleAntecedent();
  hbasahAndtdingin->joinWithAND(basah, dingin);
  FuzzyRuleConsequent *thenFanMati6 = new FuzzyRuleConsequent();
  thenFanMati6->addOutput(mati);
  FuzzyRule *fuzzyRule7 = new FuzzyRule(7, hbasahAndtdingin, thenFanMati6);
  fuzzy->addFuzzyRule(fuzzyRule7);

  //Building FuzzyRule - 8 (Kelembaban basah & suhu normal - kipas mati)
  FuzzyRuleAntecedent *hbasahAndtnormal = new FuzzyRuleAntecedent();
  hbasahAndtnormal->joinWithAND(basah, normal);
  FuzzyRuleConsequent *thenFanMati8 = new FuzzyRuleConsequent();
  thenFanMati8->addOutput(mati);
  FuzzyRule *fuzzyRule8 = new FuzzyRule(8, hbasahAndtnormal, thenFanMati8);
  fuzzy->addFuzzyRule(fuzzyRule8);

  //Building FuzzyRule - 9 (Kelembaban basah & suhu panas - kipas sebentar)
  FuzzyRuleAntecedent *hbasahAndtpanas = new FuzzyRuleAntecedent();
  hbasahAndtpanas->joinWithAND(basah, panas);
  FuzzyRuleConsequent *thenFanSebentar8 = new FuzzyRuleConsequent();
  thenFanSebentar8->addOutput(sebentar);
  FuzzyRule *fuzzyRule9 = new FuzzyRule(9, hbasahAndtpanas, thenFanSebentar8);
  fuzzy->addFuzzyRule(fuzzyRule9);

}

void loop()
{
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  
  if ((millis() - lastTime) > timerDelay) {
    // get random input
    //  sm = random(-87, 87);
//    humid = random(70, 91);
//    temp = random(30, 36);

    //  sm = map(analogRead(sensorPin), 550, 10, 0, 100);

        humid = dht.readHumidity();
        temp = dht.readTemperature();

    //  Serial.print("Soil Moisture: ");
    //  Serial.print(sm);
    //  Serial.print("%");
    Serial.print(", Humidity: ");
    Serial.print(humid);
    Serial.print("%");
    Serial.print(", and Temperature: ");
    Serial.print(temp);
    Serial.println("C");

    fuzzy->setInput(1, humid);
    fuzzy->setInput(2, temp);

    fuzzy->fuzzify();
    float output1 = fuzzy->defuzzify(1);

    Serial.print("\t\t\t\nPumping: ");
    Serial.print(output1);
    Serial.print("\n");

    // set the fields with the values
    ThingSpeak.setField(1, humid);
    ThingSpeak.setField(2, temp);
    ThingSpeak.setField(3, output1);

    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);


    Serial.print("Fired Rules: ");
    for (size_t i = 1; i <= 17; i++)
    {
      if (fuzzy->isFiredRule(i) == 1)
      {
        Serial.print(i);
        Serial.print(", ");
      }
    }
    Serial.print("\n");

    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    lastTime = millis();
  }

  //    delay(10000);
}
