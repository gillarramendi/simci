#include "simci.h"

char fps_text_1[30] = "initializing";
char fps_text_2[30] = "Status: None";

city *sim_city;

SDL_Surface *screen;

// Camera position
extern int cam_x;
extern int cam_y;
extern int cam_z;

// Game action type
int action = NONE;

// Button and keys pressed or not
bool button_1_pressed = false;
bool button_2_pressed = false;
bool button_3_pressed = false;

bool key_up_pressed = false;
bool key_down_pressed = false;
bool key_left_pressed = false;
bool key_right_pressed = false;

bool key_plus_pressed = false;
bool key_minus_pressed = false;

// Last mouse click
int last_mouse_click_x = -1;
int last_mouse_click_y = -1;

void update_fps() {
  static double last_fps_ts;
  static int frames_updated;

  double now = SDL_GetTicks();
  if (now >= last_fps_ts + SECOND) {
    double fps = frames_updated * (now - last_fps_ts) / SECOND;
    snprintf(fps_text_1, sizeof(fps_text_1), "%.2f frame/sec", fps);

    last_fps_ts = now;
    frames_updated = 0;
  }

  frames_updated++;
}

void quit_program(int code) {
  /*
   * Quit SDL so we can release the fullscreen
   * mode and restore the previous video settings,
   * etc.
   */
  delete sim_city;

  end_text();

  SDL_Quit();

  /* Exit program. */
  exit(code);
}

// Given mouse position (x,y) returns cell coords
// Otherwise koord=(-1,-1) and return -1
int cell_coords(int x, int y, int *coordx, int *coordy) {
  int id = RetrieveObjectID(x, y);
  if (id != 0) {
    coordy[0] = id % 1000 - 1;
    coordx[0] = (int)id / 1000 - 1;
    // cout << coordx[0] << ","<< coordy[0] << " clicked object on
    // position\n"<< endl;
    return 1;
  } else {
    coordx[0] = -1;
    coordy[0] = -1;
    return -1;
  }
}

void handle_key_down(SDL_Keysym *keysym) {
  switch (keysym->sym) {
  case SDLK_ESCAPE:
    quit_program(0);
    break;
  case SDLK_SPACE:
    cam_x = 0;
    cam_y = -30;
    cam_z = 20;
    break;
  case SDLK_e:
    action = NONE;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: None");
    break;
  case SDLK_u:
    action = TER_UP;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Terrain up");
    break;
  case SDLK_d:
    action = TER_DOWN;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Terrain down");
    break;
  case SDLK_l:
    action = LEVEL;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Level the land");
    break;
  case SDLK_r:
    action = RES;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Residential");
    break;
  case SDLK_c:
    action = COM;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Commerce");
    break;
  case SDLK_i:
    action = IND;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Industry");
    break;
  case SDLK_x:
    action = UNDO;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Undo");
    break;
  case SDLK_b:
    action = BUILD;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Build");
    break;
  case SDLK_k:
    action = ROAD;
    snprintf(fps_text_2, sizeof(fps_text_2), "Status: Road");
    break;
  case SDLK_RIGHT:
    key_right_pressed = true;
    cam_x++;
    break;
  case SDLK_LEFT:
    key_left_pressed = true;
    cam_x--;
    break;
  case SDLK_UP:
    key_up_pressed = true;
    cam_y++;
    break;
  case SDLK_DOWN:
    key_down_pressed = true;
    cam_y--;
    break;
  case SDLK_PLUS:
  case SDLK_KP_PLUS:
    key_plus_pressed = true;
    cam_z++;
    break;
  case SDLK_MINUS:
  case SDLK_KP_MINUS:
    key_minus_pressed = true;
    cam_z--;
    break;

  default:
    break;
  }

  printf("Camera: x:%d, y:%d, z:%d\n", cam_x, cam_y, cam_z);
}

void handle_key_up(SDL_Keysym *keysym) {
  switch (keysym->sym) {
  case SDLK_RIGHT:
    key_right_pressed = false;
    break;
  case SDLK_LEFT:
    key_left_pressed = false;
    break;
  case SDLK_UP:
    key_up_pressed = false;
    break;
  case SDLK_DOWN:
    key_down_pressed = false;
    break;
  case SDLK_PLUS:
  case SDLK_KP_PLUS:
    key_plus_pressed = false;
    break;
  case SDLK_MINUS:
  case SDLK_KP_MINUS:
    key_minus_pressed = false;
    break;
  default:
    break;
  }
}

void handle_mouse_button_down(SDL_Event event) {
  int coordx, coordy;

  printf("Mouse button %d clicked on (%d,%d)\n", event.button.button,
         event.button.x, event.button.y);
  switch (event.button.button) {
  case 1:
    button_1_pressed = true;

    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0) {
      // user clicked on a cell, store position
      last_mouse_click_x = coordx;
      last_mouse_click_y = coordy;
    }
    break;

  case 2:
    button_2_pressed = true;

    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0) {
      sim_city->create_building(coordx, coordy);
    }
    break;

  case 3:
    button_3_pressed = true;

    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0)
      sim_city->down(coordx, coordy);
    break;
  case 4:
    cam_z++;
    break;
  case 5:
    cam_z--;
    break;
  }
}

void handle_mouse_button_up(SDL_Event event) {
  int coordx, coordy;

  switch (event.button.button) {
  case 1:
    button_1_pressed = false;

    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0 &&
        last_mouse_click_x != -1 && last_mouse_click_y != -1) {
      switch (action) {
      case TER_UP:
        sim_city->up(last_mouse_click_x, last_mouse_click_y, coordx, coordy);
        sim_city->clear_color_layer();
        break;
      case TER_DOWN:
        sim_city->down(last_mouse_click_x, last_mouse_click_y, coordx, coordy);
        sim_city->clear_color_layer();
        break;
      case LEVEL:
        sim_city->level(
            last_mouse_click_x, last_mouse_click_y, coordx, coordy,
            sim_city->map[last_mouse_click_x][last_mouse_click_y].height);
        sim_city->clear_color_layer();
        break;
      case BUILD:
        if (last_mouse_click_x == coordx && last_mouse_click_y == coordy)
          sim_city->create_building(coordx, coordy);
        else
          sim_city->create_building(last_mouse_click_x, last_mouse_click_y,
                                    coordx, coordy);
        sim_city->clear_color_layer();
        break;
      case ROAD:
        sim_city->create_road(last_mouse_click_x, last_mouse_click_y, coordx,
                              coordy);
        sim_city->clear_color_layer();
        break;
      case RES:
        sim_city->set_cell_rci_type(last_mouse_click_x, last_mouse_click_y,
                                    coordx, coordy, RES);
        break;
      case COM:
        sim_city->set_cell_rci_type(last_mouse_click_x, last_mouse_click_y,
                                    coordx, coordy, COM);
        break;
      case IND:
        sim_city->set_cell_rci_type(last_mouse_click_x, last_mouse_click_y,
                                    coordx, coordy, IND);
        break;
      case UNDO:
        sim_city->set_cell_rci_type(last_mouse_click_x, last_mouse_click_y,
                                    coordx, coordy, NONE);
        break;
      default:
        break;
      }
    }
    last_mouse_click_x = -1;
    last_mouse_click_y = -1;
    break;
  case 2:
    button_2_pressed = false;
    break;
  case 3:
    button_3_pressed = false;
    break;
  default:
    break;
  }
}

void process_events(void) {
  /* Our SDL event placeholder. */
  SDL_Event event;

  /* Grab all the events off the queue. */
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
      handle_mouse_button_down(event);
      break;
    case SDL_MOUSEBUTTONUP:
      handle_mouse_button_up(event);
      break;
    case SDL_KEYDOWN:
      /* Handle key presses. */
      handle_key_down(&event.key.keysym);
      break;
    case SDL_KEYUP:
      /* Handle key up. */
      handle_key_up(&event.key.keysym);
      break;
    case SDL_QUIT:
      /* Handle quit requests (like Ctrl-c). */
      quit_program(0);
      break;
    }
  }
}

void update_status() {
  int mouse_x, mouse_y;
  int coordx, coordy;

  // move the camera
  SDL_GetMouseState(&mouse_x, &mouse_y);
  // printf("Mouse in %d %d \n", mouse_x, mouse_y);
  if (mouse_x < 10)
    cam_x--;
  if (mouse_x > SCREENWIDTH - 10)
    cam_x++;
  if (mouse_y < 10)
    cam_y++;
  if (mouse_y > SCREENHEIGHT - 10)
    cam_y--;

  // keys
  if (key_up_pressed)
    cam_y++;
  if (key_down_pressed)
    cam_y--;
  if (key_left_pressed)
    cam_x--;
  if (key_right_pressed)
    cam_x++;
  if (key_plus_pressed)
    cam_z++;
  if (key_minus_pressed)
    cam_z--;

  if (cell_coords(mouse_x, mouse_y, &coordx, &coordy) > 0 &&
      last_mouse_click_x != -1 && last_mouse_click_y != -1) {
    switch (action) {
    case TER_UP:
    case TER_DOWN:
    case LEVEL:
    case BUILD:
      sim_city->clear_color_layer();
      sim_city->update_color_layer(last_mouse_click_x, last_mouse_click_y,
                                   coordx, coordy, orange);
      break;
    case ROAD:
      sim_city->clear_color_layer();
      sim_city->update_color_layer_road(last_mouse_click_x, last_mouse_click_y,
                                        coordx, coordy);
      break;
    case RES:
      sim_city->clear_color_layer();
      sim_city->update_color_layer(last_mouse_click_x, last_mouse_click_y,
                                   coordx, coordy, dark_green);
      break;
    case COM:
      sim_city->clear_color_layer();
      sim_city->update_color_layer(last_mouse_click_x, last_mouse_click_y,
                                   coordx, coordy, blue);
      break;
    case IND:
      sim_city->clear_color_layer();
      sim_city->update_color_layer(last_mouse_click_x, last_mouse_click_y,
                                   coordx, coordy, yellow);
      break;
    case UNDO:
      sim_city->clear_color_layer();
      sim_city->update_color_layer(last_mouse_click_x, last_mouse_click_y,
                                   coordx, coordy, green);
      break;
    default:
      break;
    }
  }

  // Store current cursor position to draw it differently
  sim_city->cursor_x = coordx;
  sim_city->cursor_y = coordy;
}

int main(int argc, char *argv[]) {
  // 1. Initialize SDL video subsystem
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  // 2. Get current desktop display mode (replaces SDL_GetVideoInfo in SDL1.2)
  SDL_DisplayMode dm;
  if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
    printf("Resolution: %dx%d\n", dm.w, dm.h);
  } else {
    fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  // 3. Get pixel-format from the display mode format
  SDL_PixelFormat *fmt = SDL_AllocFormat(dm.format);
  SDL_FreeFormat(fmt);

  /*
   * 4. Set OpenGL attributes — equivalent to SDL_GL_SetAttribute calls in
   * SDL1.2 We request at least 5 bits each for red, green, and blue, a 16-bit
   * depth buffer, and double buffering.
   */
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  /*
   * 5. Create a window with an OpenGL context
   * In SDL2, SDL_SetVideoMode is replaced by SDL_CreateWindow +
   * SDL_GL_CreateContext. The SDL_WINDOW_OPENGL flag is required for OpenGL
   * rendering. Add SDL_WINDOW_FULLSCREEN for fullscreen mode if desired.
   */
  SDL_Window *window =
      SDL_CreateWindow("Simci",                   // Window title
                       SDL_WINDOWPOS_CENTERED,    // X position
                       SDL_WINDOWPOS_CENTERED,    // Y position
                       SCREENWIDTH, SCREENHEIGHT, // Window size
                       SDL_WINDOW_OPENGL          // Flags
      );

  if (!window) {
    fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  // 6. Create an OpenGL context for the window
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  if (!glcontext) {
    fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  /*
   * At this point:
   * - SDL is initialized
   * - A window is created
   * - An OpenGL context is ready
   */

  /* Set up the SDL_TTF */
  init_text();

  /*
   * At this point, we should have a properly setup
   * double-buffered window for use with OpenGL.
   */
  setup_opengl(SCREENWIDTH, SCREENHEIGHT);

  sim_city = new city(CITY_HEIGHT, CITY_WIDTH);

  /*
   * Now we want to begin our normal app process--
   * an event loop with a lot of redrawing.
   */
  while (1) {
    /*denboraren eta fps-en kontrola*/
    update_fps();
    /* Process incoming events. */
    process_events();
    /*check button and key status and perform status updates accordingly*/
    update_status();
    /* Draw the screen. */
    draw_screen();
    /* Draw text. */
    write_text(fps_text_1, true);
    write_text(fps_text_2, false);
    /* Render */
    SDL_GL_SwapWindow(window);
    /* Wait until next update */
    SDL_Delay(SECOND / 50);
  }

  /* Never reached. */
  return 0;
}
