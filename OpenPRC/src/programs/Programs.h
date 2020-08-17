/*
  OpenPRC | Programs library.
  Programmable Recrystallization Chamber
*/

#ifndef Programs_h

#define Programs_h

#include "Arduino.h"

class Programs {
public:
  Programs(bool debug);

  void begin();

private:
  bool _debug;
};

#endif
