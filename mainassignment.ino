#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <math.h>

// ------------------- Pins -------------------
#define BUZZER 5
#define LIGHT_SENSOR_PIN A0
#define TEMP_SENSOR_PIN A1      // <-- Grove Temperature Sensor
#define LED_PIN A3

// ------------------- LCD ---------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------- WiFi --------------------
// CHANGE THESE
char ssid[] = "IOT-MPSK";
char pass[] = "wlzjlzns";

// ------------------- ThingSpeak --------------------
const char tsServer[] = "api.thingspeak.com";
const int tsPort = 80;

// CHANGE THIS
String writeAPIKey = "CMKA2UURC5N9FDJ4";

WiFiClient wifi;
HttpClient tsClient(wifi, tsServer, tsPort);

// -----------------------------------------------------------------------------
// 🌡️ Read Grove Temperature (°C)
// -----------------------------------------------------------------------------
float readGroveTemperatureC(int pin) {
  int sensorValue = analogRead(pin);
  if (sensorValue == 0) return NAN;  // Avoid division by zero

  // Convert ADC reading to resistance
  float resistance = (1023.0 - sensorValue) * 10000.0 / sensorValue;

  // Apply B-parameter formula
  const float B = 3975.0;
  const float R0 = 10000.0;
  const float T0 = 298.15;  // 25°C in Kelvin

  float temperatureK = 1.0 / ((1.0 / T0) + (1.0 / B) * log(resistance / R0));
  return temperatureK - 273.15;
}

// ------------------- ThingSpeak Function --------------------
void sendToThingSpeak(int light, float temp) {

  String url = "/update?api_key=" + writeAPIKey +
               "&field1=" + String(light) +
               "&field2=" + String(temp, 2);

  tsClient.beginRequest();
  tsClient.get(url);
  tsClient.endRequest();

  int status = tsClient.responseStatusCode();
  Serial.print("ThingSpeak Status: ");
  Serial.println(status);

  String response = tsClient.responseBody();
  Serial.println(response);

  tsClient.stop();
}

// ------------------- Setup --------------------
void setup() {
  delay(500);
  Serial.begin(9600);
  delay(500);

  Serial.println("Starting system...");

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  // WiFi Connect
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.print("WiFi Connected");
  delay(1000);
  lcd.clear();
  lcd.print("Sensors Ready");
  delay(800);
}

// ------------------- Loop --------------------
void loop() {

  // Read sensors
  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  float tempC = readGroveTemperatureC(TEMP_SENSOR_PIN);

  Serial.print("Light value: ");
  Serial.println(lightValue);
  Serial.print("Temp C: ");
  Serial.println(tempC);

  // Send data to ThingSpeak
  sendToThingSpeak(lightValue, tempC);

  int threshold = 500;

  // ---------------- DARK ----------------
  if (lightValue < threshold) {
    Serial.println("Dark detected!");

    tone(BUZZER, 1000);
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dark | ");
    lcd.print(tempC, 1);
    lcd.print("C");
  }

  // ---------------- BRIGHT ----------------
  else {
    Serial.println("Bright detected!");

    noTone(BUZZER);
    digitalWrite(LED_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bright | ");
    lcd.print(tempC, 1);
    lcd.print("C");
  }

  delay(20000);  // 20s delay (ThingSpeak minimum is 15s)
}

