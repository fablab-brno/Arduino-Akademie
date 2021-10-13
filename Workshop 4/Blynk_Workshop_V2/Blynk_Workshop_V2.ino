/*************************************************************

  Sketch for Arduino Academy (lesson 4) and Blynk 2.0

 *************************************************************/

// Template ID, Device Name and Auth Token are p
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_DEVICE_NAME ""
#define BLYNK_AUTH_TOKEN "";

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "DHTesp.h" // Library Manager: DHT sensor library for ESPx

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FabLab";
char pass[] = "wearemakers";

BlynkTimer timer;
DHTesp dht;

bool ledState, potLedState, debug;  //bool variables to store states for comparison
int ledPin = 12;                    //pin for LED
int potLed = 27;                    // pin for potentiometer LED
int dhtPin = 14;                    // data pin for DHT11 sensor
int pot = 32;                       // potentiometer pin
int potValue, oldPotValue, oldMapedValue = 0;      // variable to store value from potentiometer
float temperature, humidity;        //variable to store values from DHT sensor

/* Read values from DHT sensor and send them to Blynk server*/
void readDHT() {

  humidity = dht.getHumidity(); // get humidity
  temperature = dht.getTemperature(); // get temperature (dht.readTemperature(true) for Fahrenheit)

  if (isnan(humidity) || isnan(temperature)) {
    if (debug) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, humidity); //virtual pin to send hunidity
  Blynk.virtualWrite(V2, temperature); //virtual pin to send temperature
  if (debug) {
    Serial.println("Temp is: " + String(temperature)); // print temperature to Serial line
    Serial.println("Humidity is: " + String(humidity)); // print humidity to Serial line
  }
}

/* Read values from potentiometer, map it to percent and send all of that to Blynk server*/
void readPot() {
  potValue = analogRead(pot); // read value from potentiometer pin
  if (abs(potValue - oldPotValue) > 50) {
    if (debug) {
      Serial.println("Value from potentiometer is: " + String(potValue)); // print value from potentiometer to Serial line
    }
    Blynk.virtualWrite(V3, potValue); // virtual pin for sending potentiometer value
    oldPotValue = potValue;
  }

  int mapedVal = map(potValue, 0, 4095, 0, 100); // map size of potentiometer to 100 pieces

  if (debug) {
    if ((mapedVal - oldMapedValue) > 3) {
      Serial.println("Map value is: " + String(mapedVal)); //print maped value
      Serial.println(); // print empty line
    }
    oldMapedValue = mapedVal;
  }

  Blynk.virtualWrite(V4, mapedVal); // virtual pin for sending maped value

  if (mapedVal < temperature) //if maped value is smaller than temperature we will turn OFF potLed
  {
    digitalWrite(potLed, LOW); // turn buttonLED OFF
  }
  else if (mapedVal > temperature) //if maped value is higher than temperature we will turn ON potLed
  {
    digitalWrite(potLed, HIGH); // turn buttonLED ON
  }
}

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0) {
  // Set incoming value from pin V0 to a variable
  ledState = param.asInt();

  // Update state
  Blynk.virtualWrite(V0, ledState);


  if (ledState == HIGH) {
    // // turn LED on:
    digitalWrite(ledPin, HIGH);
    if (debug) {
      Serial.println("Value of the pin is: " + String(ledState));
    }
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    if (debug) {
      Serial.println("Value of the pin is: " + String(ledState));
    }
  }
}

BLYNK_WRITE(V5) {
  // Set incoming value from pin V5 to a variable
  debug = param.asInt();

  // Update state
  Blynk.virtualWrite(V5, debug);
}

void setup() {
  // Debug console
  Serial.begin(115200);

  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080)
  Blynk.begin(auth, ssid, pass);

  pinMode(ledPin, OUTPUT);
  pinMode(potLed, OUTPUT);

  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");

  // Setup a function to be called every second
  timer.setInterval(1000L, readDHT);
  timer.setInterval(250L, readPot);
}

void loop() {
  Blynk.run();
  timer.run();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
