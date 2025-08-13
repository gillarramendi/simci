#include "simci.h"
#ifndef __CELL_H
#define __CELL_H

class cell {

public:
  cell();
  ~cell();

  int type; // Cell type: Residential, Commerce, Industry...

  structure *building;
  // structure *power_poles;
  // structure* sub1;	// Water pipes
  // structure* sub2; // Subway

  float height;
};

#endif
