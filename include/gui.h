#ifndef GUI_H
#define GUI_H

#include "./defs.h"
#include <SDL.h>
#include <SDL_image.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640

#define BOARD_SIZE 8
#define SQUARE_SIZE (WINDOW_WIDTH / BOARD_SIZE)

// Colors
#define COLOR_LIGHT_SQUARE {240, 217, 181, 255}
#define COLOR_DARK_SQUARE {181, 136, 99, 255}
#define COLOR_BLACK {0, 0, 0, 255}
#define COLOR_WHITE {255, 255, 255, 255}

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *piece_textrues[13];

  S_BOARD *board;

  int running;

} GUI_STATE;

extern int init_gui(GUI_STATE *state);
extern void cleanup_gui(GUI_STATE *state);

#endif
