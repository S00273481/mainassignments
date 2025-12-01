#include <Wire.h>
#include <rgb_lcd.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <math.h>

// ------------------- Pins -------------------
#define BUZZER 5
#define LIGHT_SENSOR_PIN A0
#define TEMP_SENSOR_PIN A1
#define LED_PIN A3
#define BUTTON_PIN 2

// ------------------- LCD -------------------
rgb_lcd lcd;

// ------------------- WiFi -------------------
char ssid[] = "IOT-MPSK";
char pass[] = "wlzjlzns";

// ------------------- ThingSpeak -------------------
const char tsServer[] = "api.thingspeak.com";
const int tsPort = 80;
String writeAPIKey = "CMKA2UURC5N9FDJ4";

WiFiClient wifi;
HttpClient tsClient(wifi, tsServer, tsPort);

// ------------------- Alarm Control -------------------
bool alarmEnabled = false;      // Starts OFF
int lastButtonState = HIGH;

// ------------------- Temperature Function -------------------
float readGroveTemperatureC(int pin) {
  int sensorValue = analogRead(pin);
  if (sensorValue <= 0) return NAN;

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

  Serial.print("ThingSpeak status: ");
  Serial.println(tsClient.responseStatusCode());
  tsClient.stop();
}

// ------------------- Setup -------------------
void setup() {
  Serial.begin(9600);
  delay(500);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize Grove LCD
  lcd.begin(16, 2);
  lcd.setRGB(0, 128, 255); // Backlight color
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

// ------------------- Loop -------------------
void loop() {
  // --- Read sensors ---
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  float tempC = readGroveTemperatureC(TEMP_SENSOR_PIN);

  Serial.print("Light: "); Serial.print(lightValue);
  Serial.print(" | Temp: "); Serial.print(tempC);
  Serial.print(" | Alarm: "); Serial.println(alarmEnabled ? "ON" : "OFF");

  // --- Button handling: toggle alarm ---
  int buttonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && buttonState == LOW) {
    alarmEnabled = !alarmEnabled;
    Serial.println(alarmEnabled ? "Alarm ENABLED" : "Alarm DISABLED");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm ");
    lcd.print(alarmEnabled ? "ON" : "OFF");
    delay(500); // Debounce
  }
  lastButtonState = buttonState;

  // --- Alarm: control buzzer + LED ---
  if (alarmEnabled) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER, 1000); // 1kHz
  } else {
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER);
  }

  // --- ThingSpeak upload ---
  sendToThingSpeak(lightValue, tempC);

  // --- LCD display ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lightValue < 500 ? "Dark" : "Bright");
  lcd.print(" | ");
  lcd.print(tempC, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Alarm:");
  lcd.print(alarmEnabled ? "ON " : "OFF");

  delay(20000); // ThingSpeak 15s minimum
}
