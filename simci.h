#ifndef __SIMCI_H
#define __SIMCI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define SCREENWIDTH 1280
#define SCREENHEIGHT 800

#define CITY_HEIGHT 20 // City height in cells
#define CITY_WIDTH 40  // City width in cells

#define SECOND 1000

class city;
class cell;
class structure;

#include "cell.h"
#include "city.h"
#include "render.h"
#include "structure.h"
#include "text.h"
// 3d model importing commented for now
// #include "l3ds.h"

// Colors
const GLubyte red[] = {255, 0, 0, 255};
const GLubyte green[] = {0, 255, 0, 255};
const GLubyte dark_green[] = {0, 153, 0, 255};
const GLubyte blue[] = {0, 0, 255, 255};
const GLubyte white[] = {255, 255, 255, 255};
const GLubyte yellow[] = {255, 255, 0, 255};
const GLubyte black[] = {0, 0, 0, 255};
const GLubyte orange[] = {255, 153, 0, 255};
const GLubyte purple[] = {102, 0, 102, 0};

// Cell type: RCI
#define RES 0  // Residential
#define COM 1  // Commerce
#define IND 2  // Industry
#define ROAD 3 // Roads

// Game action types
#define NONE 10     // None
#define TER_UP 11   // Terrain up
#define TER_DOWN 12 // Terrain down
#define LEVEL 13    // Level the land
#define BUILD 14    // Build
#define UNDO 15     // Undo

// structure types
#define HOUSE 20

#endif
