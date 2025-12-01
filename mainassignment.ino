#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <math.h>

// ------------------- Pins -------------------
#define BUZZER 5
#define LIGHT_SENSOR_PIN A0
#define TEMP_SENSOR_PIN A1
#define LED_PIN A3

// ------------------- LCD -------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------- WiFi -------------------
char ssid[] = "IOT-MPSK";          // your WiFi
char pass[] = "wlzjlzns";          // your WiFi password

// ------------------- ThingSpeak -------------------
const char tsServer[] = "api.thingspeak.com";
const int tsPort = 80;
String writeAPIKey = "CMKA2UURC5N9FDJ4";

WiFiClient wifi;
HttpClient tsClient(wifi, tsServer, tsPort);

// ------------------- Temperature Function -------------------
float readGroveTemperatureC(int pin) {
  int sensorValue = analogRead(pin);
  if (sensorValue == 0) return NAN;

  float resistance = (1023.0 - sensorValue) * 10000.0 / sensorValue;

  const float B = 3975.0;
  const float R0 = 10000.0;
  const float T0 = 298.15;

  float temperatureK = 1.0 / ((1.0 / T0) + (1.0 / B) * log(resistance / R0));
  return temperatureK - 273.15;
}

// ------------------- ThingSpeak -------------------
void sendToThingSpeak(int light, float temp) {

  String url = "/update?api_key=" + writeAPIKey +
               "&field1=" + String(light) +
               "&field2=" + String(temp, 2);

  tsClient.beginRequest();
  tsClient.get(url);
  tsClient.endRequest();

  int status = tsClient.responseStatusCode();
  Serial.print("ThingSpeak status: ");
  Serial.println(status);

  String response = tsClient.responseBody();
  Serial.println(response);

  tsClient.stop();
}

// ------------------- SETUP -------------------
void setup() {

  Serial.begin(9600);
  delay(500);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(1000);
}

// ------------------- LOOP -------------------
void loop() {

  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  float tempC = readGroveTemperatureC(TEMP_SENSOR_PIN);

  Serial.print("Light: ");
  Serial.print(lightValue);
  Serial.print(" | Temp: ");
  Serial.println(tempC);

  // --- Send to ThingSpeak ---
  sendToThingSpeak(lightValue, tempC);

  // --- LCD Display ---
  lcd.clear();
  lcd.setCursor(0, 0);

  if (lightValue < 500) {
    lcd.print("Dark | ");
  } else {
    lcd.print("Bright | ");
  }

  lcd.print(tempC, 1);
  lcd.print("C");

  delay(20000);  // ThingSpeak requires 15s minimum
}
