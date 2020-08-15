/*
  fennecPRC
  Programmable Recrystallization Chamber
  https://fennecfox.io/

  Pins:
    0  = HBRIDGE_R_EN
    1  = HBRIDGE_RPWM
    2  = HBRIDGE_L_EN
    3  = HBRIDGE_LPWM
    4  = LCD_D4
    5  = LCD_D5
    6  = LCD_D6
    7  = LCD_D7
    8  = LCD_RS
    9  = LCD_EN
    10 = LCD_BL
    11 = HBRIDGE_R_IS
    12 = HBRIDGE_L_IS
    13 = TEMPERATURE_PIN
*/

#include "src/BTS7960/BTS7960.h"
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

const float VERSION = 1.0;

// Warning: Setting to 1 will interfere with HBRIDGE_R_EN (pin 0)
// and HBRIDGE_RPWM (pin1) as they're used for serial communication.
const int DEBUG = 0;

/*
  Temperature sensor config.
*/

// Values: 9 (0.5C), 10 (0.25C), 11 (0.125C) or 12 (0.0625C) bits.
const int TEMPERATURE_PRECISION = 10;
const int TEMPERATURE_PIN = 13;

OneWire oneWire(TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress thermometerDeviceAddress;

/*
  H-bridge (for TEC) config.
*/
const int HBRIDGE_RPWM = 1;
const int HBRIDGE_R_EN = 0;
const int HBRIDGE_R_IS = 11;

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

// Soft-reset function.
void (*resetFunc)(void) = 0;

String command;

/*
  Setup.
*/
void setup() {
  if (DEBUG == 1) {
    Serial.begin(9600);

    Serial.println("--------------------------------------");
    Serial.print("fennecPRC v");
    Serial.println(VERSION, 1);
    Serial.println("Programmable Recrystallization Chamber");
    Serial.println("https://fennecfox.io/");
    Serial.println("\nSerial commands: reset, program1");
    Serial.println("--------------------------------------");
  }

  /*
    Initialise thermometer.
  */
  sensors.begin();
  sensors.getAddress(thermometerDeviceAddress, 0);
  sensors.setResolution(thermometerDeviceAddress, TEMPERATURE_PRECISION);
  if (DEBUG == 1) {
    Serial.println("Thermometer > Initialised.");
  }

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
  lcd.print(" fennecPRC v");
  lcd.print(VERSION, 1);

  if (DEBUG == 1) {
    Serial.println("LCD > Initialised.");
    Serial.println("fennecPRC > Entering main loop.");
  }
}

/*
  Main loop.
*/
void loop() {
  // Listen for serial commands.
  if (DEBUG == 1) {
    serialCommands();
  }

  /*
    Push-buttons.
  */
  int button = analogRead(0);

  // Right.
  if (button < 60) {
    lcd.print("Right");
    if (DEBUG == 1) {
      Serial.println("Button > Right pressed.");
    }
  }
  // Up.
  else if (button < 200) {
    lcd.print("Up");
    if (DEBUG == 1) {
      Serial.println("Button > Up pressed.");
    }
  }
  // Down.
  else if (button < 400) {
    lcd.print("Down");
    if (DEBUG == 1) {
      Serial.println("Button > Down pressed.");
    }
  }
  // Left.
  else if (button < 600) {
    lcd.print("Left");
    if (DEBUG == 1) {
      Serial.println("Button > Left pressed.");
    }
  }
  // Select.
  else if (button < 800) {
    lcd.print("Select");
    if (DEBUG == 1) {
      Serial.println("Button > Select pressed.");
    }
    program1();
  }
}

/*
  Serial input commands.
*/
void serialCommands() {
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();

    // "reset" command.
    if (command.equals("reset")) {
      resetFunc();
    } else if (command.equals("program1")) {
      program1();
    } else {
      if (DEBUG == 1) {
        Serial.println("fennecPRC > Invalid serial command.");
      }
    }
  }
}

/*
  Program 1 (slow recrystallization)
  Ambient temperature minus 1C per hour until 0C.
*/
void program1() {
  // LCD setup.
  lcd.setCursor(0, 0);
  lcd.print("Running (P1)... ");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");

  if (DEBUG == 1) {
    Serial.println("P1 > Running Program 1 (P1)...");
  }

  // Get initial temperature.
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  // Start cooling.
  tec.start(100, TEC_COOL);

  // Run until temperature hits 0C.
  while (tempC > 0) {
    // Listen for serial commands.
    serialCommands();

    /*
      TODO: Drop temperature by 1C per hour here.
    */

    if (tempC != DEVICE_DISCONNECTED_C) {
      lcd.setCursor(10, 1);
      lcd.print(tempC);
      lcd.print("C");

      if (DEBUG == 1) {
        Serial.print("P1 > Current temperature is: ");
        Serial.print(tempC);
        Serial.println("C");
      }
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Temp read error!");

      if (DEBUG == 1) {
        Serial.println("Thermometer > Error reading temperature!");
      }
    }
    // Wait a little before looping.
    delay(1000); // 1s

    // Update temperature.
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0);
  }
  // Stop when temperature hits 0.
  tec.stop();
  if (DEBUG == 1) {
    Serial.println("P1 > Reached 0C.");
  }
}
