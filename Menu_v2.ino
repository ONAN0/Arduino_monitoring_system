#include <LiquidCrystal.h>
#include <avr/wdt.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

const int THRESHOLD_RIGHT = 90;
const int THRESHOLD_UP = 180;
const int THRESHOLD_DOWN = 360;
const int THRESHOLD_LEFT = 540;
const int THRESHOLD_SELECT = 780;

byte arrowUp[8] = {
  0b00100,
  0b01110,
  0b11011,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte arrowDown[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b11011,
  0b01110,
  0b00100
};

int lcd_key = 0;

const int SIZE_X = 4;
const int SIZE_Y = 5;
const int SIZE_Z = 2;

String menu[SIZE_X][SIZE_Y][SIZE_Z] = {
  { { "NAN0", "" }, { "", "" }, { "", "" }, { "", "" }, { "", "" } },
  { { "Vsetko je", "" }, { "", "" }, { "", "" }, { "", "" }, { "", "" } },
  { { "Stav alarmu", "" }, { "Dym1: ", "" }, { "Voda1: ", "" }, { "Dym2: ", "" }, { "Voda2: ", "" } },
  { { "Tepl. spustenia", "" }, { "Vlhkost", "Hranica v %" }, { "Teplota", "Hranica v C " }, { "Ver 2.0", "" }, { "", "" } }
};

const char* mainMenuOptionsDown[SIZE_X] = { "", "v poriadku", "", "kurenia" };

const int alarmStateNum = 2;
const char* alarmState[alarmStateNum] = { "Neaktivny", "Aktivny" };

const int menuSize[3] = { SIZE_X, SIZE_Y, SIZE_Z };
int adressListCalculated[3] = { 0, 0, 0 };
int adressListUsed[3] = { 0, 0, 0 };

int currentIndex = 0;
int numOfOptions;

const int maxHranicneHodnotyKurenia[2] = {101, 51};
int hranicneHodnotyKurenia[2] = {70, 25};

void setup() {
  Serial.begin(9600);
  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bezpecnostny");
  lcd.setCursor(0, 1);
  lcd.print("system");
  delay(1000);
  numOfUsableOptions();
  displayMenu();
}

void loop() {
  if (read_LCD_buttons() == btnDOWN) 
  {
    adressListCalculated[currentIndex] = (adressListCalculated[currentIndex] + 1) % numOfOptions;
    adressListUsed[currentIndex] = adressListCalculated[currentIndex] + 1;
    delay(100);
  }

  if (read_LCD_buttons() == btnUP) 
  {
    adressListCalculated[currentIndex] = (adressListCalculated[currentIndex] + (numOfOptions - 1)) % numOfOptions;
    adressListUsed[currentIndex] = adressListCalculated[currentIndex] + 1;
    delay(100);
  }

  if (read_LCD_buttons() == btnSELECT)
  {
    adressListCalculated[currentIndex] = adressListUsed[currentIndex];
    currentIndex = (currentIndex + 1) % (sizeof(adressListCalculated)/sizeof(adressListCalculated[0]));
    numOfUsableOptions();
    delay(100);
  }

  if (adressListUsed[0] == 3 && (adressListUsed[1] == 1 || adressListUsed[1] == 2) && adressListUsed[2] == 1  )
  {
    changeTrigerValue();
  }

  /*
  Serial.println("----------------------");
  Serial.print("number of options: ");
  Serial.println(numOfOptions);
  Serial.print("Current index: ");
  Serial.println(currentIndex);
  Serial.print("adressListUsed[0]: ");
  Serial.println(adressListUsed[0]);
  Serial.print("adressListUsed[1]: ");
  Serial.println(adressListUsed[1]);
  Serial.print("adressListUsed[2]: ");
  Serial.println(adressListUsed[2]);
  Serial.print("adressListCalculated[0]: ");
  Serial.println(adressListCalculated[0]);
  Serial.print("adressListCalculated[1]: ");
  Serial.println(adressListCalculated[1]);
  Serial.print("adressListCalculated[2]: ");
  Serial.println(adressListCalculated[2]);
  */
  
  displayMenu();

  delay(500);
}

int read_LCD_buttons() {
  lcd_key = analogRead(0);

  if (lcd_key < THRESHOLD_RIGHT)
    return btnRIGHT;
  if (lcd_key >= THRESHOLD_RIGHT && lcd_key < THRESHOLD_UP)
    return btnUP;
  if (lcd_key >= THRESHOLD_UP && lcd_key < THRESHOLD_DOWN)
    return btnDOWN;
  if (lcd_key >= THRESHOLD_DOWN && lcd_key < THRESHOLD_LEFT)
    return btnLEFT;
  if (lcd_key >= THRESHOLD_LEFT && lcd_key < THRESHOLD_SELECT)
    return btnSELECT;

  return btnNONE;
}

void numOfUsableOptions() {
  numOfOptions = 0;

  adressListUsed[currentIndex] = 0;

  for (adressListCalculated[currentIndex] = 0; adressListCalculated[currentIndex] < menuSize[currentIndex]; adressListCalculated[currentIndex]++) {
    
    adressListUsed[currentIndex]++;
    
    if (menu[adressListUsed[0]][adressListUsed[1]][adressListUsed[2]].length() > 0) {
      numOfOptions++;
    }

  }

  adressListCalculated[currentIndex] = 0;
  adressListUsed[currentIndex] = 1;

  numOfOptions--;

  if (currentIndex == 0 || numOfOptions == 0 || numOfOptions == -1)
  {
    numOfOptions = 3;
    currentIndex = 0;
    adressListUsed[0] = 1;
    adressListUsed[1] = 0;
    adressListUsed[2] = 0;
    adressListCalculated[0] = 0;
    adressListCalculated[1] = 0;
    adressListCalculated[2] = 0;
  }
}

void displayMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menu[adressListUsed[0]][adressListUsed[1]][adressListUsed[2]]);

  if (adressListUsed[0] == 3 && (adressListUsed[1] == 1 || adressListUsed[1] == 2) && adressListUsed[2] == 1) {
    lcd.setCursor(0, 1);
    lcd.print("<");
    lcd.setCursor(6, 1);
    lcd.print(hranicneHodnotyKurenia[(adressListUsed[1] - 1)]);
    lcd.setCursor(15, 1);
    lcd.print(">");
  }
  else if (adressListUsed[1] == 0 && adressListUsed[2] == 0) {
    lcd.setCursor(0, 1);
    lcd.print(mainMenuOptionsDown[adressListUsed[0]]);
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
  else {
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
}

void changeTrigerValue()
{
  if (read_LCD_buttons() == btnRIGHT) 
  {
    hranicneHodnotyKurenia[(adressListUsed[1] - 1)] = (hranicneHodnotyKurenia[(adressListUsed[1] - 1)] + 1) % maxHranicneHodnotyKurenia[(adressListUsed[1] - 1)];
    delay(200);
  }

  if (read_LCD_buttons() == btnLEFT) 
  {
    hranicneHodnotyKurenia[(adressListUsed[1] - 1)] = (hranicneHodnotyKurenia[(adressListUsed[1] - 1)] + (maxHranicneHodnotyKurenia[(adressListUsed[1] - 1)] - 1)) % maxHranicneHodnotyKurenia[(adressListUsed[1] - 1)];
    delay(200);
  }
}