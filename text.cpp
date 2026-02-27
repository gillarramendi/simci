#include "text.h"

char fontpath[] = "Fonts/arial.ttf";

TTF_Font *font;

static int nextpoweroftwo(int x) {
  return (int)std::pow(2.0, std::ceil(std::log2(x)));
}

static void SDL_GL_RenderText(char *text, TTF_Font *fnt, SDL_Color color,
                              SDL_Rect *location) {
  SDL_Surface *initial     = TTF_RenderText_Blended(fnt, text, color);
  int          w           = nextpoweroftwo(initial->w);
  int          h           = nextpoweroftwo(initial->h);

  SDL_Surface *intermediary = SDL_CreateRGBSurface(0, w, h, 32,
                                                   0x00ff0000, 0x0000ff00,
                                                   0x000000ff, 0xff000000);
  SDL_BlitSurface(initial, 0, intermediary, 0);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
               intermediary->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture);
  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f); glVertex2f(location->x,     location->y);
  glTexCoord2f(1.0f, 1.0f); glVertex2f(location->x + w, location->y);
  glTexCoord2f(1.0f, 0.0f); glVertex2f(location->x + w, location->y + h);
  glTexCoord2f(0.0f, 0.0f); glVertex2f(location->x,     location->y + h);
  glEnd();

  glFinish();

  location->w = initial->w;
  location->h = initial->h;

  SDL_FreeSurface(initial);
  SDL_FreeSurface(intermediary);
  glDeleteTextures(1, &texture);
}

// Switch to an orthographic 2D projection for HUD rendering.
static void glEnable2D() {
  int vPort[4];
  glGetIntegerv(GL_VIEWPORT, vPort);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, vPort[2], 0, vPort[3], -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}

static void glDisable2D() {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void write_text(char *text, bool top) {
  glEnable2D();
  glDisable(GL_DEPTH_TEST);

  SDL_Color white    = {255, 255, 255, 255};
  SDL_Rect  position;
  position.x = 0;
  position.y = top ? SCREENHEIGHT - 32 : 0;

  SDL_GL_RenderText(text, font, white, &position);

  glEnable(GL_DEPTH_TEST);
  glDisable2D();
}

int init_text() {
  if (TTF_Init()) {
    printf("Error loading TTF: %s", TTF_GetError());
    return -1;
  }

  if (!(font = TTF_OpenFont(fontpath, 20))) {
    printf("Error loading font: %s", TTF_GetError());
    return -1;
  }

  return 1;
}

void end_text() {
  TTF_CloseFont(font);
  TTF_Quit();
}
