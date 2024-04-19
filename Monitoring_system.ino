#include <LiquidCrystal.h>
#include <avr/wdt.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define btnRIGHT 0
#define btnUP 1
#define btnDOWN 2
#define btnLEFT 3
#define btnSELECT 4
#define btnNONE 5

const int alarm_1 = 44;
const int alarm_2 = 46;
const int alarm_3 = 48;

const int zaplavovySensor_1 = 43;
const int zaplavovySensor_2 = 45;
const int dymovySensor_1 = 47;
const int dymovySensor_2 = 49;

const int ohrev = 50;
const int cerpadlo = 52;

const int senzorTeplomeru = A8;
const int senzorVlhkomeru = A9;

const int HRANICA_RIGHT = 90;
const int HRANICA_UP = 180;
const int HRANICA_DOWN = 360;
const int HRANICA_LEFT = 540;
const int HRANICA_SELECT = 780;

byte sipkaHore[8] = {
  0b00100,
  0b01110,
  0b11011,
  0b10001,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte sipkaDole[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10001,
  0b11011,
  0b01110,
  0b00100
};

int lcd_tlacitko = 0;

const int SIZE_X = 4;
const int SIZE_Y = 5;
const int SIZE_Z = 2;

String menu[SIZE_X][SIZE_Y][SIZE_Z] = {
  { { "NAN0", "" }, { "", "" }, { "", "" }, { "", "" }, { "", "" } },
  { { "Vsetko je", "" }, { "", "" }, { "", "" }, { "", "" }, { "", "" } },
  { { "Stav alarmu", "" }, { "Dym1: ", "" }, { "Voda1: ", "" }, { "Dym2: ", "" }, { "Voda2: ", "" } },
  { { "Ohrev a", "" }, { "Vlhkost", "Hranica v %" }, { "Teplota", "Hranica v C" }, { "Delay cerpadla", "Delay v sek." }, { "Ver 2.0", "" } }
};

char* hlavneMenuDolnaLista[SIZE_X] = { "", "v poriadku", "", "cerpadlo" };

const char* stavAlarmu[2] = { "Neaktivny", "Aktivny" };

const int velkostMenu[3] = { SIZE_X, SIZE_Y, SIZE_Z };
int zoznamAdriesNaRatanie[3] = { 0, 0, 0 };
int zoznamAdriesNaPouzitie[3] = { 0, 0, 0 };

int aktualnaPozicia = 0;
int pocetMoznosti;

const int maxHranicneHodnoty[3] = {101, 51, 4};
int hranicneHodnoty[3] = {70, 25, 2};

float napatieNaTeplomery;
float napatieNaVlhkomery;
float teplotaCelsius;
float vlhkostPercenta;

unsigned long poslednyKrat;

bool aktivneAlarmy[4];

void setup() {
  pinMode(ohrev, OUTPUT);
  pinMode(cerpadlo, OUTPUT);

  pinMode(dymovySensor_1, INPUT);
  pinMode(dymovySensor_2, INPUT);
  pinMode(zaplavovySensor_1, INPUT);
  pinMode(zaplavovySensor_2, INPUT);

  lcd.createChar(0, sipkaHore);
  lcd.createChar(1, sipkaDole);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bezpecnostny");
  lcd.setCursor(0, 1);
  lcd.print("system");
  delay(2000);
  wdt_enable(WDTO_2S);
  pocet_pouzitelnych_moznosti();
}

void loop() {

  aktivneAlarmy[0] = HIGH;
  aktivneAlarmy[1] = LOW;
  aktivneAlarmy[2] = HIGH;
  aktivneAlarmy[3] = HIGH;

  if (aktivneAlarmy[0] == LOW || aktivneAlarmy[1] == LOW)
  {
    digitalWrite(alarm_1, HIGH);
    digitalWrite(alarm_2, LOW);
    digitalWrite(alarm_3, LOW);
    digitalWrite(ohrev, LOW);
    digitalWrite(cerpadlo, LOW);
    menu[1][0][0] = "Zaznam. dym";
    hlavneMenuDolnaLista[1] = "Vsedko vypnute";
    delay(200);
  }
  else if (aktivneAlarmy[2] == LOW || aktivneAlarmy[3] == LOW)
  {
    digitalWrite(alarm_1, LOW);
    digitalWrite(alarm_2, LOW);
    digitalWrite(alarm_3, HIGH);
    digitalWrite(cerpadlo, HIGH);
    digitalWrite(ohrev, LOW);
    menu[1][0][0] = "Zaznam. voda";
    hlavneMenuDolnaLista[1] = "Cerpadlo zap.";
    delay(200);
    poslednyKrat = millis();
  }
  else
  {
    digitalWrite(alarm_1, LOW);
    digitalWrite(alarm_2, LOW);
    digitalWrite(alarm_3, LOW);
    konvertuj_na_teplotu_a_vlhkost();
    menu[1][0][0] = "Vsetko je";
    hlavneMenuDolnaLista[1] = "v poriadku";
    delay(200);
  }

  if (millis() - poslednyKrat <= (hranicneHodnoty[2]*1000)) {
    digitalWrite(cerpadlo, HIGH);
  }
  else {
    digitalWrite(cerpadlo, LOW);
  }

  if (nacitaj_LCD_tlacitko() == btnDOWN) 
  {
    zoznamAdriesNaRatanie[aktualnaPozicia] = (zoznamAdriesNaRatanie[aktualnaPozicia] + 1) % pocetMoznosti;
    zoznamAdriesNaPouzitie[aktualnaPozicia] = zoznamAdriesNaRatanie[aktualnaPozicia] + 1;
    delay(200);
  }

  if (nacitaj_LCD_tlacitko() == btnUP) 
  {
    zoznamAdriesNaRatanie[aktualnaPozicia] = (zoznamAdriesNaRatanie[aktualnaPozicia] + (pocetMoznosti - 1)) % pocetMoznosti;
    zoznamAdriesNaPouzitie[aktualnaPozicia] = zoznamAdriesNaRatanie[aktualnaPozicia] + 1;
    delay(200);
  }

  if (nacitaj_LCD_tlacitko() == btnSELECT)
  {
    zoznamAdriesNaRatanie[aktualnaPozicia] = zoznamAdriesNaPouzitie[aktualnaPozicia];
    aktualnaPozicia = (aktualnaPozicia + 1) % (sizeof(zoznamAdriesNaRatanie)/sizeof(zoznamAdriesNaRatanie[0]));
    pocet_pouzitelnych_moznosti();
    delay(200);
  }

  if (zoznamAdriesNaPouzitie[0] == 3 && (zoznamAdriesNaPouzitie[1] > 0 && zoznamAdriesNaPouzitie[1] < 4) && zoznamAdriesNaPouzitie[2] == 1  )
  {
    zmen_hranicne_hodnoty();
  }

  vypis_menu();
  
  wdt_reset();

  delay(100);
}

int nacitaj_LCD_tlacitko() {
  lcd_tlacitko = analogRead(0);

  if (lcd_tlacitko < HRANICA_RIGHT)
    return btnRIGHT;
  if (lcd_tlacitko >= HRANICA_RIGHT && lcd_tlacitko < HRANICA_UP)
    return btnUP;
  if (lcd_tlacitko >= HRANICA_UP && lcd_tlacitko < HRANICA_DOWN)
    return btnDOWN;
  if (lcd_tlacitko >= HRANICA_DOWN && lcd_tlacitko < HRANICA_LEFT)
    return btnLEFT;
  if (lcd_tlacitko >= HRANICA_LEFT && lcd_tlacitko < HRANICA_SELECT)
    return btnSELECT;

  return btnNONE;
}

void pocet_pouzitelnych_moznosti() {
  pocetMoznosti = 0;

  zoznamAdriesNaPouzitie[aktualnaPozicia] = 0;

  for (zoznamAdriesNaRatanie[aktualnaPozicia] = 0; zoznamAdriesNaRatanie[aktualnaPozicia] < velkostMenu[aktualnaPozicia]; zoznamAdriesNaRatanie[aktualnaPozicia]++) {
    
    zoznamAdriesNaPouzitie[aktualnaPozicia]++;
    
    if (menu[zoznamAdriesNaPouzitie[0]][zoznamAdriesNaPouzitie[1]][zoznamAdriesNaPouzitie[2]].length() > 0) {
      pocetMoznosti++;
    }

  }

  zoznamAdriesNaRatanie[aktualnaPozicia] = 0;
  zoznamAdriesNaPouzitie[aktualnaPozicia] = 1;

  pocetMoznosti--;

  if (aktualnaPozicia == 0 || pocetMoznosti == 0 || pocetMoznosti == -1)
  {
    pocetMoznosti = 3;
    aktualnaPozicia = 0;
    zoznamAdriesNaPouzitie[0] = 1;
    zoznamAdriesNaPouzitie[1] = 0;
    zoznamAdriesNaPouzitie[2] = 0;
    zoznamAdriesNaRatanie[0] = 0;
    zoznamAdriesNaRatanie[1] = 0;
    zoznamAdriesNaRatanie[2] = 0;
  }
}

void vypis_menu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menu[zoznamAdriesNaPouzitie[0]][zoznamAdriesNaPouzitie[1]][zoznamAdriesNaPouzitie[2]]);

  if (zoznamAdriesNaPouzitie[0] == 2 && zoznamAdriesNaPouzitie[1] != 0)
  {
    lcd.setCursor(6, 0);
    lcd.print(stavAlarmu[aktivneAlarmy[(zoznamAdriesNaPouzitie[1] - 1)]]);
  }

  if (zoznamAdriesNaPouzitie[0] == 3 && (zoznamAdriesNaPouzitie[1] > 0 && zoznamAdriesNaPouzitie[1] < 4) && zoznamAdriesNaPouzitie[2] == 1)
  {
    lcd.setCursor(0, 1);
    lcd.print("<");
    lcd.setCursor(6, 1);
    lcd.print(hranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)]);
    lcd.setCursor(15, 1);
    lcd.print(">");
  }
  else if (zoznamAdriesNaPouzitie[1] == 0 && zoznamAdriesNaPouzitie[2] == 0)
  {
    lcd.setCursor(0, 1);
    lcd.print(hlavneMenuDolnaLista[zoznamAdriesNaPouzitie[0]]);
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
  else
  {
    lcd.setCursor(15, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
}

void zmen_hranicne_hodnoty()
{
  if (nacitaj_LCD_tlacitko() == btnRIGHT) 
  {
    hranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)] = (hranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)] + 1) % maxHranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)];
    delay(100);
  }

  if (nacitaj_LCD_tlacitko() == btnLEFT) 
  {
    hranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)] = (hranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)] + (maxHranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)] - 1)) % maxHranicneHodnoty[(zoznamAdriesNaPouzitie[1] - 1)];
    delay(100);
  }
}

void konvertuj_na_teplotu_a_vlhkost()
{
  napatieNaVlhkomery = analogRead(senzorVlhkomeru) * (5.0 / 1023.0);
  napatieNaTeplomery = analogRead(senzorTeplomeru) * (5.0 / 1023.0);

  vlhkostPercenta = (napatieNaVlhkomery - 0.96) * (100 / 3.44);
  teplotaCelsius = (napatieNaTeplomery - 0.96) * (50 / (3.44));

  if (vlhkostPercenta >= hranicneHodnoty[0] || teplotaCelsius <= hranicneHodnoty[1])
  {
    digitalWrite(ohrev, HIGH);
  }
  else
  {
    digitalWrite(ohrev, LOW);
  }
}