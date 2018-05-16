#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <SimpleTimer.h>
#include "DHTesp.h"
#include "ESP32Ticker.h"
int LED = 27;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).

//char auth[] = "a9c86dba839f4863a26a7e078e48405d"; //wsh1@fl.cz : heslo
//char auth[] = "7c5375f05f7f4c2db38046cf7e2421e1"; //wsh2@fl.cz
//char auth[] = "939d5f2f780d4f0ba900680b1328b59b"; //wsh3@fl.cz
//char auth[] = "d38ed57985a04c02b083a7d53b4d4f95"; //wsh4@fl.cz
//char auth[] = "0025fa895c7743c5821682a3737bb165"; //wsh5@fl.cz
//char auth[] = "b4369c8d3cc6478683b34132d6655bf8"; //wsh6@fl.cz
//char auth[] = "de1eddc11e9047a680128ea9703d16c9"; //wsh7@fl.cz
//char auth[] = "b9180160022d464e898adc5bdae19fba"; //wsh8@fl.cz
//char auth[] = "ecc7e323529e4dd6b5dff8fb17e29463"; //wsh9@fl.cz
//char auth[] = "e2924923db264ba99ca490e2956ad860"; //wsh10@fl.cz

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "FabLab";
char pass[] = "wearemakers";

/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Comfort profile */
ComfortState cf;
/** Flag if task should run */
bool tasksEnabled = false;
/** Pin number for DHT11 data pin */
int dhtPin = 14;          // What digital pin we're connected to

DHTesp dht;
SimpleTimer timer;
TempAndHumidity newValues = dht.getTempAndHumidity();

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{


  float pot = 32;   // pin pro pripojeni potenciometru
  float valor = 0;  // promena pro hodnotu vyctenou z potenciometru
  float h = dht.getHumidity();
  float t = dht.getTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  Serial.println(t);
  Serial.println(h);

  valor = analogRead (pot); // Armazena o valor de pot na variavel valor
  Serial.println(valor);
  //valor = map(valor, 0, 1023, 0, 50);
  Blynk.virtualWrite(V7, valor);
  Blynk.virtualWrite(V8, valor);

  if (valor / 21 < (t) )
  {
    digitalWrite(LED, LOW);           //
  }
  else if (valor / 21 > (t) )
  {
    digitalWrite(LED, HIGH);           //

  }

  WidgetLED  led(V9);

  byte leds = digitalRead(LED);
  if  (leds == 1) {
    led.off();
  }
  else {
    led.on();
  }

  WidgetLCD lcd(V10);

  lcd.print(5, 0, "Blynk"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  lcd.print(2, 1, "Workshop");
  // Please use timed events when LCD printintg in void loop to avoid sending too many commands
  // It will cause a FLOOD Error, and connection will be dropped

}

void setup() {
  {
    Serial.begin(115200); // See the connection status in Serial Monitor
    Blynk.begin(auth, ssid, pass, "mtm.fablabbrno.cz", 8442);

    pinMode (LED, OUTPUT);
    initTemp();
    // Setup a function to be called every second
    timer.setInterval(1000L, sendSensor);
  }
}


void loop(){
  Blynk.run(); // Initiates Blynk
  timer.run(); // Initiates SimpleTimer
}

bool initTemp() {
  byte resultValue = 0;
  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");

  // Start task to get temperature
  xTaskCreatePinnedToCore(
      tempTask,                       /* Function to implement the task */
      "tempTask ",                    /* Name of the task */
      4000,                           /* Stack size in words */
      NULL,                           /* Task input parameter */
      5,                              /* Priority of the task */
      &tempTaskHandle,                /* Task handle. */
      1);                             /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    // Start update of environment data every 20 seconds
   // tempTicker.attach(20, triggerGetTemp);
  }
  return true;
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1) // tempTask loop
  {
    if (tasksEnabled) {
      // Get temperature values
    //  getTemperature();
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}

