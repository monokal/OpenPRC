#include "BTS7960.h"
#include "Arduino.h"

// Constructor.
// E.g. RobojaxBTS7960 tec(HBRIDGE_R_EN, HBRIDGE_RPWM, HBRIDGE_R_IS,
// HBRIDGE_L_EN, HBRIDGE_LPWM, HBRIDGE_L_IS, DEBUG);
BTS7960::BTS7960(int R_EN, int RPWM, int IS_R, int INH_L, int IN_L, int IS_L,
                 int DEBUG) {
  _debug = DEBUG;

  // Define pins.
  _R_EN = R_EN;
  _RPWM = RPWM;
  _IS_R = IS_R;

  _INH_L = INH_L;
  _IN_L = IN_L;
  _IS_L = IS_L;
}

void BTS7960::begin()
// Setup pins.
{
  // Right.
  pinMode(this->_R_EN, OUTPUT);
  pinMode(this->_RPWM, OUTPUT);
  pinMode(this->_IS_R, INPUT);

  // Left.
  pinMode(this->_INH_L, OUTPUT);
  pinMode(this->_IN_L, OUTPUT);
  pinMode(this->_IS_L, INPUT);
}

// Start function.
void BTS7960::start(int percent, int direction) {
  digitalWrite(this->_R_EN, HIGH);
  digitalWrite(this->_INH_L, HIGH);

  int pwm1Pin, pwm2Pin;

  if (direction == 1) {
    pwm1Pin = this->_IN_L;
    pwm2Pin = this->_RPWM;
  } else {
    pwm1Pin = this->_RPWM;
    pwm2Pin = this->_IN_L;
  }

  if (percent >= 0 && percent <= 100) {
    analogWrite(pwm2Pin, toPWM(percent));
  } else {
    if (this->_debug) {
      Serial.println("H-Bridge > Invalid percentage.");
    }
  }
}

// Stop function.
void BTS7960::stop() {
  digitalWrite(this->_RPWM, LOW);
  digitalWrite(this->_IN_L, LOW);

  if (this->_debug) {
    Serial.println("H-Bridge > Stopped.");
  }
}

// Converts a percentage (0-100) to 0-255.
int BTS7960::toPWM(int percent) { return map(percent, 0, 100, 0, 255); }