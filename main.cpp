#include "simci.h"

char fps_text_1[30] = "initializing";
char fps_text_2[30] = "Status: None";

city *sim_city;

// Camera position (defined in render.cpp)
extern int cam_x;
extern int cam_y;
extern int cam_z;

// Current game action (terrain, zone, build, road…)
int action = NONE;

// Mouse button states
bool button_1_pressed = false;
bool button_2_pressed = false;
bool button_3_pressed = false;

// Arrow / zoom key states
bool key_up_pressed    = false;
bool key_down_pressed  = false;
bool key_left_pressed  = false;
bool key_right_pressed = false;
bool key_plus_pressed  = false;
bool key_minus_pressed = false;

// Cell where the left mouse button was first pressed (-1 = none)
int last_mouse_click_x = -1;
int last_mouse_click_y = -1;

/* Update the FPS counter once per second. */
void update_fps() {
  static double last_fps_ts;
  static int frames_updated;

  double now = SDL_GetTicks();
  if (now >= last_fps_ts + SECOND) {
    double fps = frames_updated * (double)SECOND / (now - last_fps_ts);
    snprintf(fps_text_1, sizeof(fps_text_1), "%.2f frame/sec", fps);
    last_fps_ts    = now;
    frames_updated = 0;
  }

  frames_updated++;
}

/* Release resources and exit. */
void quit_program(int code) {
  delete sim_city;
  end_text();
  SDL_Quit();
  exit(code);
}

// Given screen position (x, y), fills coordx/coordy with the cell under the
// cursor. Returns 1 on hit, -1 if the mouse is not over any cell.
int cell_coords(int x, int y, int *coordx, int *coordy) {
  int id = RetrieveObjectID(x, y);
  if (id != 0) {
    coordy[0] = id % 1000 - 1;
    coordx[0] = (int)id / 1000 - 1;
    return 1;
  } else {
    coordx[0] = -1;
    coordy[0] = -1;
    return -1;
  }
}

void handle_key_down(SDL_Keysym *keysym) {
  std::cout << keysym->sym << " pressed\n" << std::endl;
  switch (keysym->sym) {
  case SDLK_ESCAPE:
    quit_program(0);
    break;
  case SDLK_SPACE:
    // Reset camera to default position
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

  // Arrow keys: only set the flag; update_status() moves the camera every
  // frame to avoid double-movement from key-repeat events.
  case SDLK_RIGHT:
    key_right_pressed = true;
    break;
  case SDLK_LEFT:
    key_left_pressed = true;
    break;
  case SDLK_UP:
    key_up_pressed = true;
    break;
  case SDLK_DOWN:
    key_down_pressed = true;
    break;

  case SDLK_PLUS:
  case SDLK_KP_PLUS:
  case SDL_SCANCODE_F4: // workaround for SDL + key on some Mac keyboards
    std::cout << "plus pressed\n" << std::endl;
    key_plus_pressed = true;
    break;
  case SDLK_MINUS:
  case SDLK_KP_MINUS:
    std::cout << "minus pressed\n" << std::endl;
    key_minus_pressed = true;
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
  case SDL_SCANCODE_F4:
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
  case SDL_BUTTON_LEFT:
    button_1_pressed = true;
    // Store the cell where the drag started
    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0) {
      last_mouse_click_x = coordx;
      last_mouse_click_y = coordy;
    }
    break;

  case SDL_BUTTON_MIDDLE:
    button_2_pressed = true;
    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0)
      sim_city->create_building(coordx, coordy);
    break;

  case SDL_BUTTON_RIGHT:
    button_3_pressed = true;
    if (cell_coords(event.button.x, event.button.y, &coordx, &coordy) > 0)
      sim_city->down(coordx, coordy);
    break;

  case SDL_BUTTON_X1: // scroll up → zoom in
    cam_z++;
    break;
  case SDL_BUTTON_X2: // scroll down → zoom out
    cam_z--;
    break;
  }
}

void handle_mouse_button_up(SDL_Event event) {
  int coordx, coordy;

  switch (event.button.button) {
  case SDL_BUTTON_LEFT:
    button_1_pressed = false;

    // Apply the action over the drag rectangle (click → release)
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
        // Level to the height of the first clicked cell
        sim_city->level(
            last_mouse_click_x, last_mouse_click_y, coordx, coordy,
            sim_city->map[last_mouse_click_x][last_mouse_click_y].height);
        sim_city->clear_color_layer();
        break;
      case BUILD:
        // Single click → one building; drag → fill rectangle
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
    // Reset drag origin
    last_mouse_click_x = -1;
    last_mouse_click_y = -1;
    break;

  case SDL_BUTTON_MIDDLE:
    button_2_pressed = false;
    break;
  case SDL_BUTTON_RIGHT:
    button_3_pressed = false;
    break;
  default:
    break;
  }
}

/* Drain all pending SDL events and dispatch them. */
void process_events(void) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
      handle_mouse_button_down(event);
      break;
    case SDL_MOUSEBUTTONUP:
      handle_mouse_button_up(event);
      break;
    case SDL_KEYDOWN:
      handle_key_down(&event.key.keysym);
      break;
    case SDL_KEYUP:
      handle_key_up(&event.key.keysym);
      break;
    case SDL_QUIT:
      /* Ctrl-C or window close button */
      quit_program(0);
      break;
    }
  }
}

/* Move the camera and refresh the color overlay preview every frame. */
void update_status() {
  int mouse_x, mouse_y;
  int coordx, coordy;

  // Edge-scroll: move camera when mouse is near the window border
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

  // Keyboard camera movement (held keys)
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

  // While dragging, highlight the selection rectangle with the action's color
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

  // Track cursor cell so draw_screen() can highlight it
  sim_city->cursor_x = coordx;
  sim_city->cursor_y = coordy;
}

int main(int argc, char *argv[]) {
  // 1. Initialize SDL video subsystem
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  // 2. Query the desktop resolution
  SDL_DisplayMode dm;
  if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
    printf("Resolution: %dx%d\n", dm.w, dm.h);
  } else {
    fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  // 3. Request OpenGL color/depth buffer sizes and double buffering
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // 4. Create window with an OpenGL context
  SDL_Window *window =
      SDL_CreateWindow("Simci", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_OPENGL);
  if (!window) {
    fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  SDL_GLContext glcontext = SDL_GL_CreateContext(window);
  if (!glcontext) {
    fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
    quit_program(1);
  }

  // 5. Initialize font rendering and OpenGL state
  init_text();
  setup_opengl(SCREENWIDTH, SCREENHEIGHT);

  // 6. Create the city grid
  sim_city = new city(CITY_HEIGHT, CITY_WIDTH);

  /* Main game loop */
  while (1) {
    /* Time and FPS counter update */
    update_fps();
    /* Process incoming events */
    process_events();
    /* Move camera and update color overlay preview */
    update_status();
    /* Render the scene */
    draw_screen();
    /* Draw HUD text */
    write_text(fps_text_1, true);
    write_text(fps_text_2, false);
    /*
     * Swap front and back buffers. Double buffering prevents visual tearing
     * by always presenting a fully rendered frame.
     */
    SDL_GL_SwapWindow(window);
    /* Cap to ~50 fps */
    SDL_Delay(SECOND / 50);
  }

  /* Never reached */
  return 0;
}
