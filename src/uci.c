#include "../include/defs.h"
#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER 400 * 6

void parse_go(char *line, S_SEARCHINFO *info, S_BOARD *pos) {
  int depth = -1;
  int movestogo = 30;
  int movetime = -1;
  long long time = -1;
  int inc = 0;

  char *ptr = NULL;
  info->timeset = FALSE;

  if ((ptr = strstr(line, "infinite"))) {
    ;
  }
  if ((ptr = strstr(line, "binc")) && pos->turnColor == BLACK) {
    inc = atoi(ptr + 5);
  }
  if ((ptr = strstr(line, "winc")) && pos->turnColor == WHITE) {
    inc = atoi(ptr + 5);
  }
  if ((ptr = strstr(line, "btime")) && pos->turnColor == BLACK) {
    time = atoi(ptr + 6);
  }
  if ((ptr = strstr(line, "wtime")) && pos->turnColor == WHITE) {
    time = atoi(ptr + 6);
  }
  if ((ptr = strstr(line, "movestogo"))) {
    movestogo = atoi(ptr + 10);
  }
  if ((ptr = strstr(line, "movetime"))) {
    movetime = atoi(ptr + 9);
  }
  if ((ptr = strstr(line, "depth"))) {
    depth = atoi(ptr + 6);
  }

  if (movetime != -1) {
    time = movetime;
    movestogo = 1;
  }

  info->starttime = get_time_ms();
  info->depth = depth;

  if (time != -1) {
    info->timeset = TRUE;
    time /= movestogo;
    time -= 50;
    info->stoptime = info->starttime + time + inc;
  }

  if (depth == -1) {
    info->depth = MAX_DEPTH;
  }

  printf("time:%lld start:%lld stop:%lld depth:%d timeset:%d\n", time,
         info->starttime, info->stoptime, info->depth, info->timeset);
  search_position(pos, info);
}

void parse_position(char *line_in, S_BOARD *pos) {
  line_in += 9;
  char *ptr = line_in;

  if (strncmp(line_in, "startpos", 8) == 0) {
    parse_fen(START_FEN, pos);
  } else {
    ptr = strstr(line_in, "fen");
    if (ptr == NULL) {
      parse_fen(START_FEN, pos);
    } else {
      ptr += 4;
      parse_fen(ptr, pos);
    }
  }

  ptr = strstr(line_in, "moves");
  int move;

  if (ptr != NULL) {
    ptr += 6;

    while (*ptr) {
      move = parse_move(pos, ptr);

      if (move == NOMOVE) {
        break;
      }

      make_move(pos, move);
      pos->ply = 0;

      while (*ptr && *ptr != ' ') {
        ptr++;
      }
      ptr++;
    }
  }

  print_board(pos);
}

void uci_loop(S_BOARD *pos, S_SEARCHINFO *info) {
  info->game_mode = UCI;
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  char line[INPUT_BUFFER];
  printf("id name %s\n", NAME);
  printf("id author CJB\n");
  printf("uciok\n");

  S_BOARD *board = gen_board();
  info->quit = FALSE;

  while (TRUE) {
    memset(&line[0], 0, sizeof(line));
    fflush(stdout);

    if (!fgets(line, INPUT_BUFFER, stdin)) {
      continue;
    }

    if (line[0] == '\n') {
      continue;
    }

    if (!strncmp(line, "isready", 7)) {
      printf("readyok\n");
      continue;
    } else if (!strncmp(line, "position", 8)) {
      parse_position(line, board);
    } else if (!strncmp(line, "ucinewgame", 10)) {
      parse_position("position startpos\n", board);
      /*
    reset_board(board);
    clear_pvtable(board->pvTable);
    continue;
    */
    } else if (!strncmp(line, "go", 2)) {
      parse_go(line, info, board);
    } else if (!strncmp(line, "quit", 4)) {
      info->quit = TRUE;
      break;
    } else if (!strncmp(line, "uci", 3)) {
      printf("id name %s\n", NAME);
      printf("id author CJB\n");
      printf("uciok\n");
    }

    if (info->quit) {
      break;
    }
  }
}
