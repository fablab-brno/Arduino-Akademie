#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <SPI.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h> // Library Manager: Blynk
#include <SimpleTimer.h> // install from ZIP: https://github.com/jfturcot/SimpleTimer 
#include "DHTesp.h" // Library Manager: DHT sensor library for ESPx
#include "ESP32Ticker.h" // install from ZIP: https://github.com/bertmelis/Ticker-esp32
int LED = 27;
int buttonLED = 12;

void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).

char auth[] = "put_your_uth_here!!"; 


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
  float pot = 32;   // potentiometer pin 
  float valor = 0;  // variable to store value from potentiometer
  float h = dht.getHumidity(); // get humidity
  float t = dht.getTemperature(); // get temperature (dht.readTemperature(true) for Fahrenheit)

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, h); //virtual pin to send hunidity
  Blynk.virtualWrite(V6, t); //virtual pin to send temperature
  Serial.println("Temp is: " + String(t)); // print temperature to Serial line
  Serial.println("Humidity is: " + String(h)); // print humidity to Serial line

  valor = analogRead (pot); // read value from potentiometer pin
  Serial.println("Value from potentiometer is: " + String(valor)); // print value from potentiometer to Serial line
  int valor_map = map(valor, 0, 4095, 0, 100); // map size of potentiometer to 100 pieces
  Serial.println("Map value is: " + String(valor_map)); //print maped value 
  Serial.println(); // print empty line
  Blynk.virtualWrite(V7, valor); // virtual pin for sending potentiometer value
  Blynk.virtualWrite(V8, valor_map); // virtual pin for sending maped value

  if (valor_map < (t) ) //if maped value is smaller than temperature
  {
    digitalWrite(LED, LOW); // turn buttonLED OFF  
  }
  else if (valor_map > (t) ) //if maped value is smaller than temperature
  {
    digitalWrite(LED, HIGH); // turn buttonLED ON  

  }

  WidgetLED  led(V9); // virtual pin for controling LED on breeadboard

  byte leds = digitalRead(buttonLED); // read buttonLED pin
  if  (leds == 1) { // if there is nothing.. 
    led.off(); // ..turn buttonLED OFF
  }
  else { //..else
    led.on(); //..turn buttonLED ON
  }

  WidgetLCD lcd(V10); // virtual pin for sending text to LCD widget

  lcd.print(5, 0, "Hello "); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
  lcd.print(1, 1, "Arduino Academy");
  // Please use timed events when LCD printintg in void loop to avoid sending too many commands
  // It will cause a FLOOD Error, and connection will be dropped

}

void setup() {
  {
    Serial.begin(115200); // See the connection status in Serial Monitor
    Blynk.begin(auth, ssid, pass);

    pinMode (LED, OUTPUT);
    pinMode (buttonLED, OUTPUT);
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
