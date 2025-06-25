#ifndef GUI_H
#define GUI_H

#include "./defs.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

#define BOARD_SIZE 640
#define SQUARE_SIZE (BOARD_SIZE / 8)

#define GENERAL_OFFSET 50
#define BOARD_OFFSET_X GENERAL_OFFSET
#define BOARD_OFFSET_Y GENERAL_OFFSET
#define SIDEBAR_WIDTH 250
#define SIDEBAR_X (BOARD_OFFSET_X + BOARD_SIZE + 20)

// Colors
#define COLOR_LIGHT_SQUARE {240, 217, 181, 255}
#define COLOR_DARK_SQUARE {181, 136, 99, 255}
#define COLOR_SELECTED {255, 255, 0, 150}
#define COLOR_LEGAL_MOVE {0, 255, 0, 100}
#define COLOR_LAST_MOVE {0, 255, 255, 100}
#define COLOR_CHECK {255, 0, 0, 150}
#define COLOR_WHITE {255, 255, 255, 255}
#define COLOR_BLACK {0, 0, 0, 255}
#define COLOR_BACKGROUND {45, 45, 45, 255}
#define COLOR_SIDEBAR {60, 60, 60, 255}
#define COLOR_BUTTON {80, 80, 80, 255}
#define COLOR_BUTTON_HOVER {100, 100, 100, 255}
#define COLOR_TEXT {220, 220, 220, 255}

extern const char *piece_images[];

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *piece_textures[13];
  TTF_Font *font;

  S_BOARD *board;

  int selected_sq;
  int last_from_sq;
  int last_to_sq;

  S_MOVELIST legal_moves;
  int show_legal_moves;

  int flip_board;

  char status_message[256];
  char move_history[1024];

  int mouse_x;
  int mouse_y;
  int mouse_down;
  int dragging_piece;
  int drag_piece_type;
  int drag_start_x;
  int drag_start_y;

  int running;
} GUI_STATE;

typedef struct {
  int x, y, w, h;
  char text[32];
  int hovered;
  void (*callback)(GUI_STATE *gui);
} Button;

extern int init_gui(GUI_STATE *state);
extern void cleanup_gui(GUI_STATE *state);
extern void gui_game_loop(GUI_STATE *state);

#endif
