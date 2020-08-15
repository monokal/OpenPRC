/*
  fennecPRC | BTS7960 H-Bridge library.
  Programmable Recrystallization Chamber
  https://fennecfox.io/
*/

#ifndef BTS7960_H

#define BTS7960_H

#include "Arduino.h"

class BTS7960 {
public:
  BTS7960(int RPWM, int R_EN, int R_IS, int LPWM, int L_EN, int L_IS,
          bool DEBUG);

  void begin();
  void start(int percent, int direction);
  void stop();

private:
  int toPWM(int percent);

  bool _debug;

  int _RPWM;
  int _R_EN;
  int _R_IS;

  int _LPWM;
  int _L_EN;
  int _L_IS;
};

#endif
