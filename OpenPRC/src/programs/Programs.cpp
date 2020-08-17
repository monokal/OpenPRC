/*
  OpenPRC | Programs library.
  Programmable Recrystallization Chamber
*/

#include "Programs.h"
// #include "programs/1.h"
#include "Arduino.h"

// Constructor.
Programs::Programs(bool debug) {
  _debug = debug;

  //
}

// Begin function.
void Programs::begin() {
  //

  if (this->_debug) {
    Serial.println("Programs > Initialised.");
  }
}
