#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER 5
#define LIGHT_SENSOR_PIN A0
#define LED_PIN A3

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Pins set. Initializing LCD...");

  lcd.init();
  lcd.backlight();
  Serial.println("LCD initialized.");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Debug Mode");
  delay(1000);
  lcd.clear();

  Serial.println("Setup complete.");
}

void loop() {
  Serial.println("Loop running...");
  int lightValue = analogRead(LIGHT_SENSOR_PIN);

  Serial.print("Light sensor value: ");
  Serial.println(lightValue);

  int threshold = 500;

  if (lightValue < threshold) {
    Serial.println("Dark detected.");

    tone(BUZZER, 1000);
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dark");
  } else {
    Serial.println("Bright detected.");

    noTone(BUZZER);
    digitalWrite(LED_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bright");
  }

  delay(200);
}
