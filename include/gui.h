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

} GUI_STATE;

#endif
