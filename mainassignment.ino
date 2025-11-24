#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Fire Alarm System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"

#include <WiFiS3.h>          
#include <WiFiSSLClient.h>
#include <BlynkSimpleWifi.h>   // IMPORTANT: Correct for UNO R4 WiFi
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "arduino_secrets.h"   // Wi-Fi credentials


char ssid[] = "arduino_secrets.h";
char pass[] = "tvqxskzf";

// Pins for UNO R4 WiFi (use numbers, not Dx)
#define BUZZER 5
#define LED_PIN 6
#define LIGHT_SENSOR_PIN A0

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool fireDetected = false;

void setup() {
  Serial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Connect to WiFi + Blynk Cloud
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1000);
}

void loop() {
  Blynk.run();

  int sensorValue = analogRead(LIGHT_SENSOR_PIN);
  Serial.print("Light value: ");
  Serial.println(sensorValue);

  int threshold = 500;

  // FIRE DETECTED
  if (sensorValue < threshold) {
    if (!fireDetected) {
      fireDetected = true;

      Blynk.virtualWrite(V1, "🔥 FIRE DETECTED");
      Blynk.logEvent("fire_alert", "🔥 FIRE DETECTED - immediate danger!");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("FIRE DETECTED!");

      tone(BUZZER, 1000);
    }

    digitalWrite(LED_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(150);

  } else {

    // SAFE
    if (fireDetected) {
      fireDetected = false;

      Blynk.virtualWrite(V1, "Safe");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Safe");
    }

    noTone(BUZZER);
    digitalWrite(LED_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bright");
    delay(300);
  }

  delay(100);
}
