#ifndef BTS7960_H
#define BTS7960_H

#include "Arduino.h"

class BTS7960 {
public:
  // Get pin values.
  BTS7960(int R_EN, int RPWM, int IS_R, int INH_L, int IN_L, int IS_L,
          int DEBUG)

  void begin();
  void start(int percent, int direction);
  void stop();

private:
  int toPWM(int percent);
  _debug = DEBUG;

  _R_EN = R_EN;
  _RPWM = RPWM;
  _IS_R = IS_R;

  _INH_L = INH_L;
  _IN_L = IN_L;
  _IS_L = IS_L;

  double version = 1.00;
};

#endif