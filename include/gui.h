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

#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 60
#define BUTTON_SPACING 20
#define TITLE_Y_OFFSET 150

// Colors
#define COLOR_LIGHT_SQUARE {240, 217, 181, 255}
#define COLOR_DARK_SQUARE {181, 136, 99, 255}
// Update color scheme - START
#define COLOR_SELECTED {255, 255, 0, 100}
#define COLOR_LEGAL_MOVE {0, 200, 0, 100}
#define COLOR_LAST_MOVE {66, 133, 244, 80}
#define COLOR_CHECK {255, 0, 0, 100}
// END
#define COLOR_WHITE {255, 255, 255, 255}
#define COLOR_BLACK {0, 0, 0, 255}
#define COLOR_BACKGROUND {45, 45, 45, 255}
#define COLOR_SIDEBAR {60, 60, 60, 255}
#define COLOR_BUTTON {80, 80, 80, 255}
#define COLOR_BUTTON_HOVER {100, 100, 100, 255}
#define COLOR_TEXT {220, 220, 220, 255}
#define COLOR_MENU_BG {30, 30, 40, 255}
#define COLOR_BUTTON_NORMAL {70, 70, 80, 255}
#define COLOR_BUTTON_HOVER_START {90, 90, 100, 255}
#define COLOR_BUTTON_TEXT {220, 220, 220, 255}
#define COLOR_TITLE {255, 255, 255, 255}

extern const char *piece_images[];

enum { PLAYER, ENGINE };

typedef struct GUI_STATE GUI_STATE;

typedef struct {
  int x, y, w, h;
  char text[32];
  int hovered;
  void (*callback)(GUI_STATE *gui);
} Button;

typedef struct GUI_STATE {
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

  int w_player;
  int b_player;

  int show_start_screen;
  Button start_buttons[2];
  TTF_Font *title_font;

  int engine_thinking;
  int engine_move_ready;
  int engine_move;
  long long engine_think_starttime;

  S_SEARCHINFO info;

  int running;
} GUI_STATE;

extern int init_gui(GUI_STATE *state);
extern void cleanup_gui(GUI_STATE *state);
extern void gui_game_loop(GUI_STATE *state);

#endif
