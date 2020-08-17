/*
  OpenPRC | Programs library.
  Programmable Recrystallization Chamber
*/

#include "Programs.h"
#include "Arduino.h"
// #include "programs/P1.h" // TODO.

// Constructor.
Programs::Programs(bool debug) {
  _debug = debug;

  //
}

// Begin function.
void Programs::begin() {
  if (this->_debug) {
    Serial.println("Programs > Initialised.");
  }
}

// TODO: Program 1.
void Programs::P1() {}
