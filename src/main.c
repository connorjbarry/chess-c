#include "defs.h"
#include "gui.h"
#include <stdio.h>
#include <string.h>

int main(void) {
  init();

  S_BOARD *board = gen_board();
  S_SEARCHINFO info[1];
  GUI_STATE gui = {0};

  printf("Welcome to cengine! Type 'cengine' for console mode...\n");

  char line[256];

  while (TRUE) {
    memset(&line[0], 0, sizeof(line));

    fflush(stdout);
    if (!fgets(line, 256, stdin)) {
      continue;
    }
    if (line[0] == '\n') {
      continue;
    }
    if (!strncmp(line, "uci", 3)) {
      uci_loop(board, info);
      if (info->quit == TRUE) {
        break;
      }
      continue;
    } else if (!strncmp(line, "xboard", 6)) {
      xboard_loop(board, info);
      if (info->quit == TRUE) {
        break;
      }
      continue;
    } else if (!strncmp(line, "cengine", 7)) {
      console_loop(board, info);
      if (info->quit == TRUE) {
        break;
      }
      continue;
    } else if (!strncmp(line, "gui", 3)) {
      parse_fen(START_FEN, board);
      gui.board = board;

      if (!init_gui(&gui)) {
        break;
      }

      printf("\ncengine GUI started. Press ESC or close window "
             "to exit.\n");

      gui_game_loop(&gui);

      cleanup_gui(&gui);
      printf("GUI closed. Exiting...\n");
      break;
    } else if (!strncmp(line, "quit", 4)) {
      break;
    }
  }

  return 0;
}
