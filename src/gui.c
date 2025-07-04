#include "../include/gui.h"
#include "defs.h"

const char *piece_images[] = {NULL,
                              "white_pawn.png",
                              "white_knight.png",
                              "white_bishop.png",
                              "white_rook.png",
                              "white_queen.png",
                              "white_king.png",
                              "black_pawn.png",
                              "black_knight.png",
                              "black_bishop.png",
                              "black_rook.png",
                              "black_queen.png",
                              "black_king.png"};

/***********
 * HELPERS *
 ***********/
int _get_piece_at_square(S_BOARD *board, int display_row, int display_col) {
  int file = display_col;
  int rank = 7 - display_row;

  int sq120 = square_to_index(file, rank);

  return board->pieces[sq120];
}

int _get_sq_from_coords(const int x, const int y, const int flip_board) {
  if (x < BOARD_OFFSET_X || x >= BOARD_OFFSET_X + BOARD_SIZE ||
      y < BOARD_OFFSET_Y || y >= BOARD_OFFSET_Y + BOARD_SIZE) {
    return NO_SQ;
  }

  int file = (x - BOARD_OFFSET_X) / SQUARE_SIZE;
  int rank = (y - BOARD_OFFSET_Y) / SQUARE_SIZE;

  int display_file = flip_board ? 7 - file : file;
  int display_rank = flip_board ? rank : 7 - rank;

  return square_to_index(display_file, display_rank);
}

void _make_move(GUI_STATE *state, int from_sq, int to_sq) {
  // Find matching move in legal moves
  for (int i = 0; i < state->legal_moves.count; i++) {
    int move = state->legal_moves.moves[i].move;
    if (move_from_sq(move) == from_sq && move_to_sq(move) == to_sq) {
      if (make_move(state->board, move)) {
        state->last_from_sq = from_sq;
        state->last_to_sq = to_sq;
        state->selected_sq = NO_SQ;

        // Update move history
        char move_str[10];
        strcpy(move_str, move_to_string(move));
        if (strlen(state->move_history) > 0) {
          strcat(state->move_history, " ");
        }
        strcat(state->move_history, move_str);

        generate_all_moves(state->board, &state->legal_moves);

        // Check game state
        if (state->legal_moves.count == 0) {
          if (square_attacked(state->board->kingSqs[state->board->turnColor],
                              state->board->turnColor ^ 1, state->board)) {
            strcpy(state->status_message, "Checkmate!");
          } else {
            strcpy(state->status_message, "Stalemate!");
          }
        } else if (square_attacked(
                       state->board->kingSqs[state->board->turnColor],
                       state->board->turnColor ^ 1, state->board)) {
          strcpy(state->status_message, "Check!");
        } else {
          strcpy(state->status_message, "Move played");
        }
        return;
      }
    }
  }

  strcpy(state->status_message, "Illegal move!");
  state->selected_sq = NO_SQ;
}

int _is_legal_move(GUI_STATE *state, int from_sq, int to_sq) {
  for (int i = 0; i < state->legal_moves.count; i++) {
    int move = state->legal_moves.moves[i].move;
    if (move_from_sq(move) == from_sq && move_to_sq(move) == to_sq) {
      return 1;
    }
  }
  return 0;
}

/******************
 * BUTTON CALLBACKS *
 ******************/

void start_user_vs_user(GUI_STATE *gui) {
  gui->w_player = PLAYER;
  gui->b_player = PLAYER;
  gui->show_start_screen = 0;
  strcpy(gui->status_message, "User vs User - White to move");
}

void start_user_vs_engine(GUI_STATE *gui) {
  gui->w_player = PLAYER;
  gui->b_player = ENGINE;
  gui->show_start_screen = 0;
  strcpy(gui->status_message, "User vs Engine - You are White");
}

/******************
 * START SCREEN   *
 ******************/

void init_start_screen(GUI_STATE *state) {
  state->show_start_screen = 1;

  // Initialize title font
  state->title_font = TTF_OpenFont("arial.ttf", 48);
  if (!state->title_font) {
    state->title_font =
        TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 48);
  }
  if (!state->title_font) {
    // Fallback to regular font with larger size
    state->title_font = state->font;
  }

  // Initialize buttons
  int center_x = WINDOW_WIDTH / 2;
  int start_y = WINDOW_HEIGHT / 2 - 30;

  // User vs User button
  state->start_buttons[0].x = center_x - BUTTON_WIDTH / 2;
  state->start_buttons[0].y = start_y;
  state->start_buttons[0].w = BUTTON_WIDTH;
  state->start_buttons[0].h = BUTTON_HEIGHT;
  strcpy(state->start_buttons[0].text, "User vs User");
  state->start_buttons[0].hovered = 0;
  state->start_buttons[0].callback = start_user_vs_user;

  // User vs Engine button
  state->start_buttons[1].x = center_x - BUTTON_WIDTH / 2;
  state->start_buttons[1].y = start_y + BUTTON_HEIGHT + BUTTON_SPACING;
  state->start_buttons[1].w = BUTTON_WIDTH;
  state->start_buttons[1].h = BUTTON_HEIGHT;
  strcpy(state->start_buttons[1].text, "User vs Engine");
  state->start_buttons[1].hovered = 0;
  state->start_buttons[1].callback = start_user_vs_engine;
}

void draw_start_button(GUI_STATE *state, Button *button) {
  SDL_Rect button_rect = {button->x, button->y, button->w, button->h};

  // Choose color based on hover state
  SDL_Color bg_color = button->hovered ? (SDL_Color)COLOR_BUTTON_HOVER_START
                                       : (SDL_Color)COLOR_BUTTON_NORMAL;

  // Draw button background
  SDL_SetRenderDrawColor(state->renderer, bg_color.r, bg_color.g, bg_color.b,
                         bg_color.a);
  SDL_RenderFillRect(state->renderer, &button_rect);

  // Draw button border
  SDL_SetRenderDrawColor(state->renderer, 150, 150, 150, 255);
  SDL_RenderDrawRect(state->renderer, &button_rect);

  // Draw button text
  if (state->font) {
    SDL_Color text_color = COLOR_BUTTON_TEXT;
    SDL_Surface *text_surface =
        TTF_RenderText_Blended(state->font, button->text, text_color);

    if (text_surface) {
      SDL_Texture *text_texture =
          SDL_CreateTextureFromSurface(state->renderer, text_surface);

      if (text_texture) {
        // Center text in button
        int text_x = button->x + (button->w - text_surface->w) / 2;
        int text_y = button->y + (button->h - text_surface->h) / 2;

        SDL_Rect text_rect = {text_x, text_y, text_surface->w, text_surface->h};
        SDL_RenderCopy(state->renderer, text_texture, NULL, &text_rect);

        SDL_DestroyTexture(text_texture);
      }
      SDL_FreeSurface(text_surface);
    }
  }
}

void render_start_screen(GUI_STATE *state) {
  // Clear screen with menu background
  SDL_Color bg = COLOR_MENU_BG;
  SDL_SetRenderDrawColor(state->renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(state->renderer);

  // Draw title
  SDL_Color title_color = COLOR_TITLE;
  const char *title_text = "Chess Engine";

  TTF_Font *font_to_use = state->title_font ? state->title_font : state->font;
  if (font_to_use) {
    SDL_Surface *title_surface =
        TTF_RenderText_Blended(font_to_use, title_text, title_color);

    if (title_surface) {
      SDL_Texture *title_texture =
          SDL_CreateTextureFromSurface(state->renderer, title_surface);

      if (title_texture) {
        int title_x = (WINDOW_WIDTH - title_surface->w) / 2;
        int title_y = TITLE_Y_OFFSET;

        SDL_Rect title_rect = {title_x, title_y, title_surface->w,
                               title_surface->h};
        SDL_RenderCopy(state->renderer, title_texture, NULL, &title_rect);

        SDL_DestroyTexture(title_texture);
      }
      SDL_FreeSurface(title_surface);
    }
  }

  // Draw subtitle
  const char *subtitle_text = "Choose Game Mode";
  if (state->font) {
    SDL_Surface *subtitle_surface =
        TTF_RenderText_Blended(state->font, subtitle_text, title_color);

    if (subtitle_surface) {
      SDL_Texture *subtitle_texture =
          SDL_CreateTextureFromSurface(state->renderer, subtitle_surface);

      if (subtitle_texture) {
        int subtitle_x = (WINDOW_WIDTH - subtitle_surface->w) / 2;
        int subtitle_y = TITLE_Y_OFFSET + 80;

        SDL_Rect subtitle_rect = {subtitle_x, subtitle_y, subtitle_surface->w,
                                  subtitle_surface->h};
        SDL_RenderCopy(state->renderer, subtitle_texture, NULL, &subtitle_rect);

        SDL_DestroyTexture(subtitle_texture);
      }
      SDL_FreeSurface(subtitle_surface);
    }
  }

  // Draw buttons
  for (int i = 0; i < 2; i++) {
    draw_start_button(state, &state->start_buttons[i]);
  }

  // Draw instructions at bottom
  const char *instruction_text = "Press ESC to quit";
  if (state->font) {
    SDL_Color instruction_color = {180, 180, 180, 255};
    SDL_Surface *instruction_surface = TTF_RenderText_Blended(
        state->font, instruction_text, instruction_color);

    if (instruction_surface) {
      SDL_Texture *instruction_texture =
          SDL_CreateTextureFromSurface(state->renderer, instruction_surface);

      if (instruction_texture) {
        int instruction_x = (WINDOW_WIDTH - instruction_surface->w) / 2;
        int instruction_y = WINDOW_HEIGHT - 50;

        SDL_Rect instruction_rect = {instruction_x, instruction_y,
                                     instruction_surface->w,
                                     instruction_surface->h};
        SDL_RenderCopy(state->renderer, instruction_texture, NULL,
                       &instruction_rect);

        SDL_DestroyTexture(instruction_texture);
      }
      SDL_FreeSurface(instruction_surface);
    }
  }

  SDL_RenderPresent(state->renderer);
}

int point_in_button(Button *button, int x, int y) {
  return x >= button->x && x <= button->x + button->w && y >= button->y &&
         y <= button->y + button->h;
}

void handle_start_screen_events(GUI_STATE *state) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      state->running = 0;
      break;

    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        state->running = 0;
      }
      break;

    case SDL_MOUSEMOTION:
      // Update hover states
      for (int i = 0; i < 2; i++) {
        state->start_buttons[i].hovered = point_in_button(
            &state->start_buttons[i], event.motion.x, event.motion.y);
      }
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        // Check if any button was clicked
        for (int i = 0; i < 2; i++) {
          if (point_in_button(&state->start_buttons[i], event.button.x,
                              event.button.y)) {
            if (state->start_buttons[i].callback) {
              state->start_buttons[i].callback(state);
            }
            return;
          }
        }
      }
      break;
    }
  }
}

void cleanup_start_screen(GUI_STATE *state) {
  if (state->title_font && state->title_font != state->font) {
    TTF_CloseFont(state->title_font);
    state->title_font = NULL;
  }
}

/***********
 * LOADERS *
 ***********/

int load_piece_images(GUI_STATE *state) {
  for (int i = 0; i < 13; i++) {
    state->piece_textures[i] = NULL;
  }

  for (int i = 1; i < 13; i++) {
    if (piece_images[i] != NULL) {
      char _img_path[256];
      snprintf(_img_path, sizeof(_img_path), "./assets/pieces/%s",
               piece_images[i]);
      SDL_Surface *surface = IMG_Load(_img_path);
      if (!surface) {
        printf("Failed to load image %s: %s\n", piece_images[i],
               IMG_GetError());
        return 0;
      }

      state->piece_textures[i] =
          SDL_CreateTextureFromSurface(state->renderer, surface);
      SDL_FreeSurface(surface);

      if (!state->piece_textures[i]) {
        printf("Failed to create texture for %s: %s\n", piece_images[i],
               SDL_GetError());
        return 0;
      }
    }
  }

  return 1;
}

void free_piece_images(GUI_STATE *state) {
  for (int i = 0; i < 13; i++) {
    if (state->piece_textures[i]) {
      SDL_DestroyTexture(state->piece_textures[i]);
      state->piece_textures[i] = NULL;
    }
  }
}

/******************
 * RENDER METHODS *
 ******************/

void _draw_piece_fallback(GUI_STATE *state, int piece, int x, int y) {
  // Simple fallback drawing when font is not available
  SDL_Rect piece_rect = {x + SQUARE_SIZE / 4, y + SQUARE_SIZE / 4,
                         SQUARE_SIZE / 2, SQUARE_SIZE / 2};

  // Set color based on piece color
  if (piece >= wP && piece <= wK) {
    SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, 255); // White pieces
  } else if (piece >= bP && piece <= bK) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 255); // Black pieces
  }

  SDL_RenderFillRect(state->renderer, &piece_rect);

  // Draw a simple border
  SDL_SetRenderDrawColor(state->renderer, 128, 128, 128, 255);
  SDL_RenderDrawRect(state->renderer, &piece_rect);
}

void draw_pieces(GUI_STATE *state) {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      int piece = _get_piece_at_square(state->board, row, col);

      if (piece != EMPTY && state->piece_textures[piece]) {
        int x = col * SQUARE_SIZE;
        int y = row * SQUARE_SIZE;

        // Get the texture dimensions
        int tex_w, tex_h;
        SDL_QueryTexture(state->piece_textures[piece], NULL, NULL, &tex_w,
                         &tex_h);

        // Scale the piece to fit nicely in the square (leave some padding)
        int piece_size = (int)(SQUARE_SIZE * 0.9); // 90% of square size
        int offset = (SQUARE_SIZE - piece_size) / 2 + GENERAL_OFFSET;

        SDL_Rect dest_rect = {x + offset, y + offset, piece_size, piece_size};

        SDL_RenderCopy(state->renderer, state->piece_textures[piece], NULL,
                       &dest_rect);

      } else if (piece != EMPTY) {
        // Fallback if image didn't load
        _draw_piece_fallback(state, piece, col * SQUARE_SIZE,
                             row * SQUARE_SIZE);
      }
    }
  }
}

void draw_square(GUI_STATE *state, int file, int rank) {
  int display_file = state->flip_board ? 7 - file : file;
  int display_rank = state->flip_board ? rank : 7 - rank;
  int sq120 = square_to_index(display_file, display_rank);

  int x = BOARD_OFFSET_X + file * SQUARE_SIZE;
  int y = BOARD_OFFSET_Y + rank * SQUARE_SIZE;

  SDL_Rect square = {x, y, SQUARE_SIZE, SQUARE_SIZE};

  SDL_Color base_color = ((display_file + display_rank) % 2 == 0)
                             ? (SDL_Color)COLOR_LIGHT_SQUARE
                             : (SDL_Color)COLOR_DARK_SQUARE;
  SDL_SetRenderDrawColor(state->renderer, base_color.r, base_color.g,
                         base_color.b, base_color.a);
  SDL_RenderFillRect(state->renderer, &square);

  SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_BLEND);

  // Last move
  if (sq120 == state->last_from_sq || sq120 == state->last_to_sq) {
    SDL_Color last_color = COLOR_LAST_MOVE;
    SDL_SetRenderDrawColor(state->renderer, last_color.r, last_color.g,
                           last_color.b, last_color.a);
    SDL_RenderFillRect(state->renderer, &square);
  }

  // Selected square
  if (sq120 == state->selected_sq) {
    SDL_Color sel_color = COLOR_SELECTED;
    SDL_SetRenderDrawColor(state->renderer, sel_color.r, sel_color.g,
                           sel_color.b, sel_color.a);
    SDL_RenderFillRect(state->renderer, &square);
  }

  // Legal moves
  if (state->show_legal_moves && state->selected_sq != NO_SQ) {
    if (_is_legal_move(state, state->selected_sq, sq120)) {
      SDL_Color legal_color = COLOR_LEGAL_MOVE;
      SDL_SetRenderDrawColor(state->renderer, legal_color.r, legal_color.g,
                             legal_color.b, legal_color.a);
      SDL_RenderFillRect(state->renderer, &square);
    }
  }

  // Check highlight
  if (state->board->pieces[sq120] == wK || state->board->pieces[sq120] == bK) {
    int king_color = PieceCol[state->board->pieces[sq120]];
    if (square_attacked(sq120, king_color ^ 1, state->board)) {
      SDL_Color check_color = COLOR_CHECK;
      SDL_SetRenderDrawColor(state->renderer, check_color.r, check_color.g,
                             check_color.b, check_color.a);
      SDL_RenderFillRect(state->renderer, &square);
    }
  }

  SDL_SetRenderDrawBlendMode(state->renderer, SDL_BLENDMODE_NONE);
}

void render_text(GUI_STATE *state, const char *text, int x, int y,
                 SDL_Color color, TTF_Font *font) {
  if (!font || !text)
    return;

  SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, color);
  if (!text_surface)
    return;

  SDL_Texture *text_texture =
      SDL_CreateTextureFromSurface(state->renderer, text_surface);
  if (!text_texture) {
    SDL_FreeSurface(text_surface);
    return;
  }

  SDL_Rect dest_rect = {x, y, text_surface->w, text_surface->h};
  SDL_RenderCopy(state->renderer, text_texture, NULL, &dest_rect);

  SDL_DestroyTexture(text_texture);
  SDL_FreeSurface(text_surface);
}

void draw_board(GUI_STATE *state) {
  SDL_Rect board_rect = {BOARD_OFFSET_X - 5, BOARD_OFFSET_Y - 5,
                         BOARD_SIZE + 10, BOARD_SIZE + 10};
  SDL_Color border = COLOR_BLACK;
  SDL_SetRenderDrawColor(state->renderer, border.r, border.g, border.b,
                         border.a);
  SDL_RenderFillRect(state->renderer, &board_rect);

  // Draw squares and pieces
  for (int rank = 0; rank < 8; rank++) {
    for (int file = 0; file < 8; file++) {
      draw_square(state, file, rank);
    }
  }

  // Draw coordinates
  SDL_Color coord_color = COLOR_TEXT;
  for (int i = 0; i < 8; i++) {
    char coord[2];

    // File labels
    coord[0] = 'a' + (state->flip_board ? 7 - i : i);
    coord[1] = '\0';
    render_text(state, coord,
                BOARD_OFFSET_X + i * SQUARE_SIZE + SQUARE_SIZE / 2 - 5,
                BOARD_OFFSET_Y + BOARD_SIZE + 10, coord_color, state->font);

    // Rank labels
    coord[0] = '1' + (state->flip_board ? i : 7 - i);
    render_text(state, coord, BOARD_OFFSET_X - 20,
                BOARD_OFFSET_Y + i * SQUARE_SIZE + SQUARE_SIZE / 2 - 10,
                coord_color, state->font);
  }
}

void draw_sidebar(GUI_STATE *state) {
  SDL_Rect sidebar_rect = {SIDEBAR_X, BOARD_OFFSET_Y, SIDEBAR_WIDTH,
                           BOARD_SIZE};
  SDL_Color sidebar_color = COLOR_SIDEBAR;
  SDL_SetRenderDrawColor(state->renderer, sidebar_color.r, sidebar_color.g,
                         sidebar_color.b, sidebar_color.a);
  SDL_RenderFillRect(state->renderer, &sidebar_rect);

  int y_pos = BOARD_OFFSET_Y + 20;
  SDL_Color text_color = COLOR_TEXT;

  // Title
  render_text(state, "cengine - Chess Engine", SIDEBAR_X + 10, y_pos,
              text_color, state->font);
  y_pos += 40;

  // Game status
  char status_text[64];
  snprintf(status_text, sizeof(status_text), "Turn: %s",
           state->board->turnColor == WHITE ? "White" : "Black");
  render_text(state, status_text, SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 25;

  // Move counter
  snprintf(status_text, sizeof(status_text), "Move: %d",
           (state->board->historyPly / 2) + 1);
  render_text(state, status_text, SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 25;

  // Fifty move rule
  snprintf(status_text, sizeof(status_text), "50-move: %d",
           state->board->fiftyMoveCtr);
  render_text(state, status_text, SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 25;

  // Castling rights
  char castling[8] = "";
  if (state->board->castlePermission & WKCA) {
    strcat(castling, "K");
  }
  if (state->board->castlePermission & WQCA) {
    strcat(castling, "Q");
  }
  if (state->board->castlePermission & BKCA) {
    strcat(castling, "k");
  }
  if (state->board->castlePermission & BQCA) {
    strcat(castling, "q");
  }
  if (strlen(castling) == 0) {
    strcpy(castling, "-");
  }

  snprintf(status_text, sizeof(status_text), "Castling: %s", castling);
  render_text(state, status_text, SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 40;

  // Status message
  render_text(state, "Status:", SIDEBAR_X + 10, y_pos, text_color, state->font);
  y_pos += 20;
  render_text(state, state->status_message, SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 40;

  // Controls
  render_text(state, "Controls:", SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 25;
  render_text(state, "F - Flip board", SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 20;
  render_text(state, "N - New game", SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 20;
  render_text(state, "U - Undo move", SIDEBAR_X + 10, y_pos, text_color,
              state->font);
  y_pos += 20;
  render_text(state, "L - Toggle legal moves", SIDEBAR_X + 10, y_pos,
              text_color, state->font);
}

void render(GUI_STATE *state) {
  SDL_Color bg = COLOR_BACKGROUND;
  SDL_SetRenderDrawColor(state->renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(state->renderer);

  // Draw board and pieces
  draw_board(state);
  draw_sidebar(state);
  draw_pieces(state);

  // Present everything
  SDL_RenderPresent(state->renderer);
}

/******************
 * EVENT HANDLERS *
 ******************/

void handle_square_click(GUI_STATE *state, int sq) {
  if (sq == NO_SQ) {
    state->selected_sq = NO_SQ;
    return;
  }

  if (state->selected_sq == NO_SQ) {
    // First click - select piece
    if (state->board->pieces[sq] != EMPTY &&
        PieceCol[state->board->pieces[sq]] == state->board->turnColor) {
      state->selected_sq = sq;
    }
  } else {
    // Second click - try to move
    if (sq == state->selected_sq) {
      // Deselect
      state->selected_sq = NO_SQ;
    } else {
      _make_move(state, state->selected_sq, sq);
    }
  }
}

void handle_events(GUI_STATE *state) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      state->running = 0;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        state->running = 0;
      }
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        state->mouse_down = 1;
        state->mouse_x = event.button.x;
        state->mouse_y = event.button.y;

        int square = _get_sq_from_coords(state->mouse_x, state->mouse_y,
                                         state->flip_board);

        if (square != NO_SQ && state->board->pieces[square] != EMPTY) {
          // start dragging
          if (PieceCol[state->board->pieces[square]] ==
              state->board->turnColor) {
            // state->dragging_piece = 1;
            state->drag_piece_type = state->board->pieces[square];
            state->drag_start_x = state->mouse_x;
            state->drag_start_y = state->mouse_y;
            // state->selected_sq = square;

            generate_all_moves(state->board, &state->legal_moves);
          }
        }
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        state->mouse_down = 0;

        int sq = _get_sq_from_coords(event.button.x, event.button.y,
                                     state->flip_board);

        if (state->dragging_piece) {
          if (sq != NO_SQ && state->selected_sq != NO_SQ) {
            _make_move(state, state->selected_sq, sq);
          }
          state->dragging_piece = 0;
          state->selected_sq = NO_SQ;
        } else {
          handle_square_click(state, sq);
        }
        break;
      }
    case SDL_MOUSEMOTION:
      state->mouse_x = event.motion.x;
      state->mouse_y = event.motion.y;

      // Only start dragging if mouse has moved significantly and we have a
      // selected piece
      if (state->mouse_down && !state->dragging_piece) {
        int dx = state->mouse_x - state->drag_start_x;
        int dy = state->mouse_y - state->drag_start_y;
        int distance = dx * dx + dy * dy;
        if (distance > 25) { // 5 pixel threshold squared
          state->dragging_piece = 1;
          state->selected_sq = _get_sq_from_coords(
              state->drag_start_x, state->drag_start_y, state->flip_board);
        }
      }
      break;
    }
  }
}

/**************
 * MAIN FUNCS *
 **************/

int init_gui(GUI_STATE *state) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return 0;
  }

  int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (!(IMG_Init(img_flags) & img_flags)) {
    printf("SDL_image initialization failed: %s\n", IMG_GetError());
    SDL_Quit();
    return 0;
  }

  // Create window
  state->window = SDL_CreateWindow("cengine GUI", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                   WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!state->window) {
    printf("Window creation failed: %s\n", SDL_GetError());
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  state->renderer =
      SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
  if (!state->renderer) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(state->window);
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  if (!load_piece_images(state)) {
    printf("Failed to load piece images\n");
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  if (TTF_Init() == -1) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return 0;
  }

  state->font = TTF_OpenFont("arial.ttf", 16);

  // If font loading fails, try system fonts
  if (!state->font) {
    state->font =
        TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 16);
  }

  // Initialize GUI state
  state->selected_sq = NO_SQ;
  state->last_from_sq = NO_SQ;
  state->last_to_sq = NO_SQ;
  state->show_legal_moves = 1;
  state->flip_board = 0;
  state->mouse_down = 0;
  state->dragging_piece = 0;
  state->running = 1;
  state->show_start_screen = 1;
  state->title_font = NULL;

  strcpy(state->status_message, "Welcome to Chess Engine");
  strcpy(state->move_history, "");

  generate_all_moves(state->board, &state->legal_moves);

  return 1;
}

void cleanup_gui(GUI_STATE *state) {
  free_piece_images(state);

  if (state->font) {
    TTF_CloseFont(state->font);
  }

  if (state->renderer) {
    SDL_DestroyRenderer(state->renderer);
  }
  if (state->window) {
    SDL_DestroyWindow(state->window);
  }
  IMG_Quit();
  SDL_Quit();
}

void _update_gui_state(GUI_STATE *state) {
  if (state->show_start_screen) {
    handle_start_screen_events(state);
  } else {
    handle_events(state);
  }
}

void _render_gui_state(GUI_STATE *state) {
  if (state->show_start_screen) {
    render_start_screen(state);
  } else {
    render(state);
  }
}

void gui_game_loop(GUI_STATE *state) {
  init_start_screen(state);

  while (state->running) {
    _update_gui_state(state);
    _render_gui_state(state);
    SDL_Delay(16); // 60 ish fps
  }

  cleanup_start_screen(state);
}
