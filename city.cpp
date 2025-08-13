#include "city.h"

city::city() { city(CITY_HEIGHT, CITY_WIDTH); }

city::city(int xsize, int ysize) {

  this->xsize = xsize;
  this->ysize = ysize;

  int i, j;

  // Init color_layer
  color_layer = new const GLubyte **[xsize];
  for (i = 0; i < xsize; i++)
    color_layer[i] = new const GLubyte *[ysize];

  for (i = 0; i < xsize; i++)
    for (j = 0; j < ysize; j++) {
      color_layer[i][j] = NULL; // Init empty
    }

  // Init cell map
  map = new cell *[xsize + 1];
  for (i = 0; i <= xsize; i++)
    map[i] = new cell[ysize + 1];

  // Set cursor out of screen
  cursor_x = -1;
  cursor_y = -1;
}

city::~city() {
  for (int i = 0; i <= xsize; i++)
    delete[] map[i];

  delete[] map;

  for (int i = 0; i < xsize; i++)
    delete[] color_layer[i];

  delete[] color_layer;
}

void city::update_color_layer(int start_x, int start_y, int end_x, int end_y,
                              const GLubyte *color) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2; i++)
    for (j = y1; j <= y2; j++) {
      color_layer[i][j] = color;
    }
}

void city::clear_color_layer(int start_x, int start_y, int end_x, int end_y) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2; i++)
    for (j = y1; j <= y2; j++) {
      color_layer[i][j] = NULL;
    }
}

void city::clear_color_layer() {
  clear_color_layer(0, 0, xsize - 1, ysize - 1);
}

void city::set_cell_rci_type(int start_x, int start_y, int end_x, int end_y,
                             int rci_type) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2; i++)
    for (j = y1; j <= y2; j++)
      map[i][j].type = rci_type;
}

void city::up(int start_x, int start_y, int end_x, int end_y) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2 + 1; i++)
    for (j = y1; j <= y2 + 1; j++)
      map[i][j].height += 0.25;
}

void city::down(int x, int y) { this->down(x, y, x, y); }

void city::down(int start_x, int start_y, int end_x, int end_y) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2 + 1; i++)
    for (j = y1; j <= y2 + 1; j++)
      map[i][j].height -= 0.25;
}

void city::level(int start_x, int start_y, int end_x, int end_y, float height) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2 + 1; i++)
    for (j = y1; j <= y2 + 1; j++)
      map[i][j].height = height;
}

void city::create_building(int x, int y) {
  structure *str = new structure();
  map[x][y].building = str;
}

void city::create_building(int start_x, int start_y, int end_x, int end_y) {
  int x1 = std::min(start_x, end_x);
  int x2 = std::max(start_x, end_x);
  int y1 = std::min(start_y, end_y);
  int y2 = std::max(start_y, end_y);

  int i, j;
  for (i = x1; i <= x2; i++)
    for (j = y1; j <= y2; j++) {
      structure *str = new structure();
      map[i][j].building = str;
    }
}

void city::update_color_layer_road(int start_x, int start_y, int end_x,
                                   int end_y) {
  if (start_x == end_x && start_y == end_y)
    color_layer[start_x][start_y] = black;
  else {
    color_layer[start_x][start_y] = black;

    if (end_x < start_x && end_y < start_y) {
      if (start_x - end_x > start_y - end_y)
        update_color_layer_road(start_x - 1, start_y, end_x, end_y);
      else
        update_color_layer_road(start_x, start_y - 1, end_x, end_y);
    } else if (end_x >= start_x && end_y < start_y) {
      if (end_x - start_x > start_y - end_y)
        update_color_layer_road(start_x + 1, start_y, end_x, end_y);
      else
        update_color_layer_road(start_x, start_y - 1, end_x, end_y);
    } else if (end_x < start_x && end_y >= start_y) {
      if (start_x - end_x > end_y - start_y)
        update_color_layer_road(start_x - 1, start_y, end_x, end_y);
      else
        update_color_layer_road(start_x, start_y + 1, end_x, end_y);
    } else /*if(end_x>=start_x && buky>=start_y)*/
    {
      if (end_x - start_x > end_y - start_y)
        update_color_layer_road(start_x + 1, start_y, end_x, end_y);
      else
        update_color_layer_road(start_x, start_y + 1, end_x, end_y);
    }
  }
}

void city::create_road(int start_x, int start_y, int end_x, int end_y) {
  if (start_x == end_x && start_y == end_y)
    map[start_x][start_y].type = ROAD;
  else {
    map[start_x][start_y].type = ROAD;

    if (end_x < start_x && end_y < start_y) {
      if (start_x - end_x > start_y - end_y)
        create_road(start_x - 1, start_y, end_x, end_y);
      else
        create_road(start_x, start_y - 1, end_x, end_y);
    } else if (end_x >= start_x && end_y < start_y) {
      if (end_x - start_x > start_y - end_y)
        create_road(start_x + 1, start_y, end_x, end_y);
      else
        create_road(start_x, start_y - 1, end_x, end_y);
    } else if (end_x < start_x && end_y >= start_y) {
      if (start_x - end_x > end_y - start_y)
        create_road(start_x - 1, start_y, end_x, end_y);
      else
        create_road(start_x, start_y + 1, end_x, end_y);
    } else /*if(end_x>=start_x && end_y>=start_y)*/
    {
      if (end_x - start_x > end_y - start_y)
        create_road(start_x + 1, start_y, end_x, end_y);
      else
        create_road(start_x, start_y + 1, end_x, end_y);
    }
  }
}

bool city::is_flat(int x, int y) {
  bool is_flat = true;
  float height = map[x][y].height;

  if (map[x + 1][y].height != height)
    is_flat = false;
  if (map[x][y + 1].height != height)
    is_flat = false;
  if (map[x + 1][y + 1].height != height)
    is_flat = false;

  return is_flat;
}
