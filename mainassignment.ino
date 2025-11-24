#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// ------------------- Pins -------------------
#define BUZZER 5
#define LIGHT_SENSOR_PIN A0
#define LED_PIN A3

// ------------------- LCD ---------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);   // If LCD does not work, change to 0x3F

// ------------------- WiFi --------------------
char ssid[] = "arduino_secrets.h";         // CHANGE THIS
char pass[] = "tvqxskzf";     // CHANGE THIS

// ------------------- ThingSpeak --------------------
const char tsServer[] = "api.thingspeak.com";
const int tsPort = 80;
String writeAPIKey = "CMKA2UURC5N9FDJ4";   // CHANGE THIS

WiFiClient wifi;
HttpClient tsClient(wifi, tsServer, tsPort);

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

  // Connect WiFi
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
  lcd.print("Light Sensor");
}

// ------------------- ThingSpeak Function --------------------
void sendToThingSpeak(int value) {
  String url = "/update?api_key=" + writeAPIKey + "&field1=" + String(value);

  tsClient.beginRequest();
  tsClient.get(url);
  tsClient.endRequest();

  int status = tsClient.responseStatusCode();
  Serial.print("ThingSpeak Status: ");
  Serial.println(status);

  tsClient.stop();
}

// ------------------- Loop --------------------
void loop() {
  Serial.println("Loop running...");

  int lightValue = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Light value: ");
  Serial.println(lightValue);

  // Send value to ThingSpeak
  sendToThingSpeak(lightValue);

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
    lcd.print("Dark");
  }

  // ---------------- BRIGHT ----------------
  else {
    Serial.println("Bright detected!");

    noTone(BUZZER);
    digitalWrite(LED_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bright");
  }

  delay(20000);   // ThingSpeak requires 15s minimum between updates
}
