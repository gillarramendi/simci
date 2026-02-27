#include "cell.h"

cell::cell() {
  type = NONE;
  height = 0.0f;
  building = nullptr;
}

cell::~cell() { delete building; }
