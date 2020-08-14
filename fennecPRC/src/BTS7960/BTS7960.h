#ifndef BTS7960_H

#define BTS7960_H

#include "Arduino.h"

class BTS7960 {
public:
  BTS7960(int R_EN, int RPWM, int IS_R, int INH_L, int IN_L, int IS_L,
          int DEBUG);

  void begin();
  void start(int percent, int direction);
  void stop();

private:
  int toPWM(int percent);

  int _debug;

  int _R_EN;
  int _RPWM;
  int _IS_R;

  int _INH_L;
  int _IN_L;
  int _IS_L;

  double version = 1.00;
};

#endif
