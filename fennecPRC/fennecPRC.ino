/*
  fennecPRC
  Programmable Recrystallization Chamber
  https://fennecfox.io/
*/

#include "src/BTS7960/BTS7960.h"
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

const float VERSION = 1.0;
const int DEBUG = 0;

/*
  Temperature sensor config.
*/
const int TEMPERATURE_PIN = 13;
const int TEMPERATURE_PRECISION =
    10; // 9 (0.5C), 10 (0.25C), 11 (0.125C) or 12 (0.0625C) bits.

OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerDeviceAddress;

/*
  H-bridge (for TEC) config.
*/
const int HBRIDGE_RPWM = 5;
const int HBRIDGE_R_EN = 4;
const int HBRIDGE_R_IS = 8;
const int HBRIDGE_LPWM = 3;
const int HBRIDGE_L_EN = 2;
const int HBRIDGE_L_IS = 12;

const int TEC_COOL = 1;
const int TEC_HEAT = 0;

BTS7960 tec(HBRIDGE_R_EN, HBRIDGE_RPWM, HBRIDGE_R_IS, HBRIDGE_L_EN,
            HBRIDGE_LPWM, HBRIDGE_L_IS, DEBUG);

/*
  LCD shield config.
*/
const int LCD_RS = 8;
const int LCD_EN = 9;
const int LCD_D4 = 4;
const int LCD_D5 = 5;
const int LCD_D6 = 6;
const int LCD_D7 = 7;
const int LCD_BL = 10; // Backlight.

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

/*
  Setup.
*/
void setup() {
  Serial.begin(9600);

  Serial.println("--------------------------------------");
  Serial.print("fennecPRC v");
  Serial.println(VERSION, 1);
  Serial.println("Programmable Recrystallization Chamber");
  Serial.println("https://fennecfox.io/");
  Serial.println("--------------------------------------");

  /*
    Initialise thermometer.
  */
  sensors.begin();
  sensors.getAddress(thermometerDeviceAddress, 0);
  sensors.setResolution(thermometerDeviceAddress, TEMPERATURE_PRECISION);

  /*
    Initialise TEC.
  */
  tec.begin();
  tec.stop(); // Stop the TEC on reset.

  /*
    Initialise LCD.
  */
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("fennecPRC v");
  lcd.print(VERSION, 1);
}

/*
  Main loop.
*/
void loop() {
  /*
    Display temperature.
  */
  sensors.requestTemperatures();

  float tempC = sensors.getTempCByIndex(0);

  if (tempC != DEVICE_DISCONNECTED_C) {
    lcd.setCursor(0, 1);
    lcd.print("Temp: ");

    lcd.setCursor(10, 1);
    lcd.print(tempC);
    lcd.print("C");
  } else {
    lcd.print("Temp error!");
  }

  /*
    Push-buttons.
  */
  int button = analogRead(0);

  // Right.
  if (button < 60) {
    lcd.print("Right");
  }
  // Up.
  else if (button < 200) {
    lcd.print("Up");
  }
  // Down.
  else if (button < 400) {
    lcd.print("Down");
  }
  // Left.
  else if (button < 600) {
    lcd.setCursor(0, 0);
    lcd.print("Heating(100%)...");
    tec.stop();
    tec.start(100, TEC_HEAT);
  }
  // Select.
  else if (button < 800) {
    lcd.setCursor(0, 0);
    lcd.print("Cooling(100%)...");
    tec.stop();
    tec.start(100, TEC_COOL);
  }
}
