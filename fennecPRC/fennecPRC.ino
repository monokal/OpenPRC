/*
  fennecPRC
  Programmable Recrystallization Chamber
  https://fennecfox.io/

  Pins:
    0  = HBRIDGE_R_EN (OR serial if DEBUG)
    1  = HBRIDGE_RPWM (OR serial if DEBUG)
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
    A0 = LCD shield buttons
    A1 = BUZZER
*/

#include "src/BTS7960/BTS7960.h"
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>

const float VERSION = 1.0;

// Warning: Setting to false will interfere with HBRIDGE_R_EN (pin 0)
// and HBRIDGE_RPWM (pin1) as they're used for serial communication.
const bool DEBUG = false;
String command;

// Disable the buzzer.
const bool SILENT = false;

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

/*
  Buzzer config.
*/
const int BUZZER = A1;

/*
  Setup.
*/
void setup() {
  if (DEBUG) {
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
    Initialise buzzer.
  */
  pinMode(BUZZER, OUTPUT);
  buzzer("ok");

  /*
    Initialise thermometer.
  */
  sensors.begin();
  sensors.getAddress(thermometerDeviceAddress, 0);
  sensors.setResolution(thermometerDeviceAddress, TEMPERATURE_PRECISION);
  if (DEBUG) {
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

  lcd.setCursor(0, 1);
  lcd.print("     READY      ");

  if (DEBUG) {
    Serial.println("LCD > Initialised.");
    Serial.println("fennecPRC > Ready.");
  }
}

/*
  Main loop.
*/
void loop() {
  // Listen for serial commands.
  if (DEBUG) {
    serialCommands();
  }

  /*
    Push-buttons.
  */
  int button = analogRead(0);

  // Right.
  if (button < 60) {
    lcd.print("Right");
    if (DEBUG) {
      Serial.println("Button > Right pressed.");
    }
  }
  // Up.
  else if (button < 200) {
    lcd.print("Up");
    if (DEBUG) {
      Serial.println("Button > Up pressed.");
    }
  }
  // Down.
  else if (button < 400) {
    lcd.print("Down");
    if (DEBUG) {
      Serial.println("Button > Down pressed.");
    }
  }
  // Left.
  else if (button < 600) {
    lcd.print("Left");
    if (DEBUG) {
      Serial.println("Button > Left pressed.");
    }
  }
  // Select.
  else if (button < 800) {
    lcd.print("Select");
    if (DEBUG) {
      Serial.println("Button > Select pressed.");
    }
    // TODO: Menu system for
    program1();
  }
}

/*
  Soft-reset.
*/
void (*resetFunc)(void) = 0;

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
      if (DEBUG) {
        Serial.println("fennecPRC > Invalid serial command.");
      }
    }
  }
}

/*
  Buzzer.
*/
void buzzer(String type) {
  if (!SILENT) {
    // Error - 3 long A4 notes.
    if (type == "error") {
      for (int i = 0; i < 3; i++) {
        tone(BUZZER, 440, 1000);
        delay(2000);
        noTone(BUZZER);
      }
      // OK - 1 short B7 note.
    } else if (type == "ok") {
      tone(BUZZER, 3951, 250);
      // Countdown - Error then OK.
    } else if (type == "countdown") {
      buzzer("error");
      buzzer("ok");
    } else {
      if (DEBUG) {
        Serial.println("fennecPRC > Invalid buzzer type.");
      }
    }
  }
}

/*
  Program 1 (slow recrystallization)
  Ambient temperature minus 1C per hour until 0C.
*/
void program1() {
  // LCD/Buzzer acknowledgement.
  lcd.setCursor(0, 0);
  lcd.print("Starting (P1)... ");

  buzzer("countdown");

  lcd.setCursor(0, 0);
  lcd.print("Running (P1)... ");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");

  if (DEBUG) {
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
      lcd.setCursor(6, 1);
      lcd.print("    ");
      lcd.print(tempC);
      lcd.print("C");

      if (DEBUG) {
        Serial.print("P1 > Current temperature is: ");
        Serial.print(tempC);
        Serial.println("C");
      }
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Temp read error!");

      if (DEBUG) {
        Serial.println("Thermometer > Error reading temperature!");
      }

      buzzer("error");
    }
    // Wait a little before looping.
    delay(1000); // 1s

    // Update temperature.
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0);
  }
  // Stop when temperature hits 0.
  tec.stop();
  if (DEBUG) {
    Serial.println("P1 > Reached 0C.");
    buzzer("ok");
  }
}
