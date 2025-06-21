#include "../include/defs.h"
#include <stdio.h>
#include <stdlib.h>

#define WAC1                                                                   \
  "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"

int main(void) {
  init();

  uci_loop();
  /**
  S_BOARD *board = gen_board();
  S_MOVELIST m_list[1];
  S_SEARCHINFO info[1];

  // parse_fen(START_FEN, board);
  parse_fen(WAC1, board);

  char input[6];
  int move = NOMOVE;
  int pv_num = 0;
  int max = 0;

  while (TRUE) {
    print_board(board);

    printf("Please enter a move > ");
    fgets(input, 6, stdin);

    if (input[0] == 'q') {
      break;
    } else if (input[0] == 't') {
      undo_move(board);
      continue;
    } else if (input[0] == 'p') {
      perft_test(board, 4);
    } else if (input[0] == 's') {
      info->depth = 6;
      info->starttime = get_time_ms();
      info->stoptime = get_time_ms() + 200000;
      search_position(board, info);
    } else {
      move = parse_move(board, input);
      if (move != NOMOVE) {
        store_pvmove(board, move);
        make_move(board, move);
      } else {
        printf("Invalid move");
      }
    }

    fflush(stdin);
  }
  */

  return 0;
}
