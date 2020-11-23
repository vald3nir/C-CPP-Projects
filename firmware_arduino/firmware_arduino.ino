#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"

/*
  Bibliotecas utilizadas:

  LCD:                  https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads/NewliquidCrystal_1.3.4.zip
  Sensor de corrente:   https://github.com/openenergymonitor/EmonLib

  Pinagem do I2C ->     Verde: A5, Azul: A4 Branco: 0V Roxo: 5V

*/

// ============================================================================================================

#define NUMERO_AMOSTRAS 1480
#define CONSTANTE_CONVERSAO 4
#define TENSAO_RESIDENCIAL 220

// ============================================================================================================
// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Manipulador do sensor de corrente
EnergyMonitor SCT013;

double potencia, corrente;

// ============================================================================================================

void print_lcd(String l1, String l2)
{
  lcd.setCursor(0, 0);
  lcd.print(l1);
  lcd.setCursor(0, 1);
  lcd.print(l2);
}

// ============================================================================================================

void setup_lcd()
{
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  print_lcd("    MOACI    ", "   UFC - 2019   ");
  delay(2000);
}

// ============================================================================================================

void setup()
{
  setup_lcd();
  //Pino analógico conectado ao SCT-013 100A
  SCT013.current(A0, CONSTANTE_CONVERSAO);
  Serial.begin(9600);
}

void loop()
{
  corrente = SCT013.calcIrms(NUMERO_AMOSTRAS);
  potencia = corrente * TENSAO_RESIDENCIAL;
  print_lcd("( A ) => " + String(corrente, 6), "( W ) => " + String(potencia, 6));
  delay(1000);
}
