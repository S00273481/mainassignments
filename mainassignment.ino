#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // Include the LCD library for I2C

#define BUZZER 5              // Pin connected to the buzzer
#define LIGHT_SENSOR_PIN A0   // Pin connected to the light sensor
#define LED_PIN A3            // Pin connected to the LED

// Initialize the LCD (address, columns, rows)
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // LCD address can be 0x3F or 0x27, depends on your display

void setup() {
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);  // To monitor light sensor values
  
  // Initialize the LCD
  lcd.begin();
  lcd.setBacklight(LOW); // Initially, turn off the backlight
  lcd.clear();  // Clear the screen

  lcd.setCursor(0, 0);  // Set cursor to top left corner
  lcd.print("Light Sensor");  // Display a message on LCD
}

void loop() {
  int lightValue = analogRead(LIGHT_SENSOR_PIN);  // Read light sensor (0-1023)
  Serial.println(lightValue);  // Print to Serial Monitor for debugging

  int threshold = 500;  // Adjust based on your environment

  if (lightValue < threshold) {
    // It is dark (sensor covered) → buzzer ON continuously
    tone(BUZZER, 1000);

    // Blink the LED
    digitalWrite(LED_PIN, HIGH);  // LED ON
    delay(250);                   // Wait 250ms
    digitalWrite(LED_PIN, LOW);   // LED OFF
    delay(250);                   // Wait 250ms

    // Display on LCD: "It's Dark"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("hello");

    // Optionally, turn on the backlight
    lcd.setBacklight(HIGH);

  } else {
    // It is bright → turn off LED and buzzer
    noTone(BUZZER);              // Buzzer OFF
    digitalWrite(LED_PIN, LOW);  // LED OFF

    // Display on LCD: "It's Bright"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("It's Bright");

    // Optionally, turn off the backlight
    lcd.setBacklight(LOW);
  }

  delay(100);  // Small delay to avoid rapid looping
}
