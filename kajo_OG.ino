#include <LiquidCrystal.h>
#include <avr/wdt.h> // Include the watchdog timer library

// digitálne piny, komunikáčne cez ktoré sa posielaju informacie z arduina na dislej
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int alarm_1 = 44;
int alarm_2 = 46;
int alarm_3 = 48;

int ohrev = 50;
int cerpadlo = 52;

// Define input pins for logic signals
int zapl_1 = 43;
int zapl_2 = 45;
int dym_1 = 47;
int dym_2 = 49;

int lcd_key = 0;
int adc_key_in = 0;

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

// funkcia ktorá niečo vykonáva
int read_LCD_buttons()
{
  adc_key_in = analogRead(0); // read the value from the sensor
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1500)
    return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)
    return btnRIGHT;
  if (adc_key_in < 195)
    return btnUP;
  if (adc_key_in < 380)
    return btnDOWN;
  if (adc_key_in < 500)
    return btnLEFT;
  if (adc_key_in < 700)
    return btnSELECT;
  return btnNONE; // when all others fail, return this...
}

void setup() // po spustení prebehni iba raz
{
  // Set up LED pins
  pinMode(ohrev, OUTPUT);
  pinMode(cerpadlo, OUTPUT);

  // Set up input pins
  pinMode(dym_1, INPUT);
  pinMode(dym_2, INPUT);
  pinMode(zapl_1, INPUT);
  pinMode(zapl_2, INPUT);

  lcd.begin(16, 2); // start the library
  lcd.setCursor(0, 0);
  lcd.print("Bezpecnostny system"); // print a simple message

  wdt_enable(WDTO_2S);
}

void loop()
{
  // Read input pins
  int state1 = digitalRead(dym_1);
  int state2 = digitalRead(dym_2);
  int state3 = digitalRead(zapl_1);
  int state4 = digitalRead(zapl_2);

  // Turn on corresponding LED based on input pins
  if (state1 == LOW || state2 == LOW)
  {
    // Turn on red LED
    digitalWrite(alarm_1, HIGH);
    digitalWrite(alarm_2, LOW);
    digitalWrite(alarm_3, LOW);
    digitalWrite(ohrev, LOW);
    digitalWrite(cerpadlo, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zaznamenany dym");
    lcd.setCursor(0, 1);
    lcd.print("Vsedko vypnute");
    delay(2000);
  }
  else if (state3 == LOW || state4 == LOW)
  {
    // Turn on blue LED
    digitalWrite(alarm_1, LOW);
    digitalWrite(alarm_2, LOW);
    digitalWrite(alarm_3, HIGH);
    digitalWrite(ohrev, LOW);
    digitalWrite(cerpadlo, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zaznamenana voda");
    lcd.setCursor(0, 1);
    lcd.print("Zapnute cerpadlo");
    delay(2000);
  }
  else
  {
    // Turn off both LEDs
    digitalWrite(alarm_1, LOW);
    digitalWrite(alarm_2, LOW);
    digitalWrite(alarm_3, LOW);
    digitalWrite(ohrev, HIGH);
    digitalWrite(cerpadlo, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zapnute kurenie");
    lcd.setCursor(0, 1);
    lcd.print("XX *C");
  }

  wdt_reset();
  
  delay(100); // Delay for stability
}
