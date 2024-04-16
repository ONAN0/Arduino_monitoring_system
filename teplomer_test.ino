#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int lcdCols = 16;
const int lcdRows = 2;

const int TempSensorPin = A7;

void convertToTemperatureAndVoltage(int sensorValue, float &temperature_C, float &voltage) {
  voltage = sensorValue * (5.0 / 1023.0);
  temperature_C = (voltage - 0.96) * (50 / (3.44));
}

void setup() {
  lcd.begin(lcdCols, lcdRows);
}

void loop() {
  int sensorValue = analogRead(TempSensorPin);

  float temperature_C;
  float voltage;

  convertToTemperatureAndVoltage(sensorValue, temperature_C, voltage);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("U (V):");
  lcd.print(voltage);
  lcd.setCursor(0, 1);
  lcd.print("Temp. (");
  lcd.print((char) 0xDF);
  lcd.print("C):");
  lcd.print(temperature_C);

  delay(250);
}
