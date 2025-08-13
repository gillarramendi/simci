#ifndef __CITY_H
#define __CITY_H

#include "simci.h"

class city {

public:
  city();
  city(int, int);
  ~city();

  // City size.
  int xsize;
  int ysize;

  // cell map[xsize + 1][ysize + 1]
  cell **map;

  // Temporal color layer, while mouse is in momement and the change is not
  // definitive const GLubyte* temp_kolore[xsize][ysize];
  const GLubyte ***color_layer;

  // Store current cursor position to draw it differently. -1 if out of screen
  int cursor_x;
  int cursor_y;

  // clear color_layer as there were no color
  void clear_color_layer();
  void clear_color_layer(int start_x, int start_y, int end_x, int end_y);
  void update_color_layer(int start_x, int start_y, int end_x, int end_y,
                          const GLubyte *color);

  // Update cell type
  void set_cell_rci_type(int start_x, int start_y, int end_x, int end_y,
                         int rci_type);

  // Terrain modifications
  void up(int start_x, int start_y, int end_x, int end_y);
  void down(int x, int y);
  void down(int start_x, int start_y, int end_x, int end_y);
  void level(int start_x, int start_y, int end_x, int end_y, float height);

  // Create structures
  void create_building(int x, int y);
  void create_building(int start_x, int start_y, int end_x, int end_y);

  void update_color_layer_road(int start_x, int start_y, int end_x, int end_y);
  void create_road(int start_x, int start_y, int end_x, int end_y);

  bool is_flat(int x, int y);
};

#endif
