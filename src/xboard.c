#include "../include/defs.h"
#include <stdio.h>
#include <string.h>

int three_fold_rep(const S_BOARD *pos) {
  int idx = 0;
  int rep = 0;

  for (idx = 0; idx < pos->historyPly; ++idx) {
    if (pos->history[idx].posKey == pos->posKey) {
      rep++;
    }
  }

  return rep;
}

int draw_material(const S_BOARD *pos) {
  if (pos->pieceNum[wP] || pos->pieceNum[bP]) {
    return FALSE;
  }
  if (pos->pieceNum[wQ] || pos->pieceNum[bQ] || pos->pieceNum[wR] ||
      pos->pieceNum[bR]) {
    return FALSE;
  }
  if (pos->pieceNum[wB] > 1 || pos->pieceNum[bB] > 1) {
    return FALSE;
  }
  if (pos->pieceNum[wN] > 1 || pos->pieceNum[bN] > 1) {
    return FALSE;
  }
  if (pos->pieceNum[wN] && pos->pieceNum[wB]) {
    return FALSE;
  }
  if (pos->pieceNum[wN] && pos->pieceNum[bB]) {
    return FALSE;
  }

  return TRUE;
}

int check_result(S_BOARD *pos) {
  if (pos->fiftyMoveCtr > 100) {
    printf("1/2-1/2 {fifty move rule (claimed by cengine)}\n");
    return TRUE;
  }

  if (three_fold_rep(pos) >= 2) {
    printf("1/2-1/2 {3-fold repetition (claimed by cengine)}\n");
    return TRUE;
  }

  if (draw_material(pos) == TRUE) {
    printf("1/2-1/2 {insufficient material (claimed by cengine)}\n");
    return TRUE;
  }

  S_MOVELIST m_list[1];
  generate_all_moves(pos, m_list);

  int move_num = 0;
  int found = 0;

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    if (!make_move(pos, m_list->moves[move_num].move)) {
      continue;
    }

    found++;
    undo_move(pos);
    break;
  }

  if (found != 0) {
    return FALSE;
  }

  int in_check =
      square_attacked(pos->kingSqs[pos->turnColor], pos->turnColor ^ 1, pos);

  if (in_check == TRUE) {
    if (pos->turnColor == WHITE) {
      printf("0-1 {black mates (claimed by cengine)}\n");
      return TRUE;
    } else {
      printf("0-1 {white mates (claimed by cengine)}\n");
      return TRUE;
    }
  } else {
    printf("\n1/2-1/2 {stalemate (claimed by cengine)}\n");
    return TRUE;
  }

  return FALSE;
}

void xboard_loop(S_BOARD *pos, S_SEARCHINFO *info) {
  info->game_mode = XBOARD;
  info->post_thinking = TRUE;
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  int depth = -1;
  int movestogo[2] = {30, 30};
  int movetime = -1;
  long long time = -1;
  int inc = 0;
  int sec = 0;

  int engine_side = BOTH;
  int time_left;
  int mps;
  int move = NOMOVE;
  int idx;
  int score;
  char in_buf[80];
  char command[80];

  while (TRUE) {
    fflush(stdout);

    if (pos->turnColor == engine_side && check_result(pos) == FALSE) {
      info->starttime = get_time_ms();
      info->depth = depth;

      if (time != -1) {
        info->timeset = TRUE;
        time /= movestogo[pos->turnColor];
        time -= 50;
        info->stoptime = info->starttime + time + inc;
      }
      if (depth == -1 || depth > MAX_DEPTH) {
        info->depth = MAX_DEPTH;
      }

      printf("time:%lld start:%lld stop:%lld depth:%d timeset:%d movestogo:%d "
             "mps:%d\n",
             time, info->starttime, info->stoptime, info->depth, info->timeset,
             movestogo[pos->turnColor], mps);
      search_position(pos, info);

      if (mps != 0) {
        movestogo[pos->turnColor ^ 1]--;
        if (movestogo[pos->turnColor ^ 1] < 1) {
          movestogo[pos->turnColor ^ 1] = mps;
        }
      }
    }

    fflush(stdout);

    memset(&in_buf[0], 0, sizeof(in_buf));
    fflush(stdout);
    if (!fgets(in_buf, 80, stdin)) {
      continue;
    }

    sscanf(in_buf, "%s", command);

    if (!strcmp(command, "quit")) {
      break;
    }

    if (!strcmp(command, "force")) {
      engine_side = BOTH;
      continue;
    }

    if (!strcmp(command, "protover")) {
      printf("feature ping=1 setboard=1 colors=0 usermove=1\n");
      printf("feature done=1\n");
      continue;
    }

    if (!strcmp(command, "sd")) {
      sscanf(in_buf, "sd %d", &depth);
      continue;
    }

    if (!strcmp(command, "st")) {
      sscanf(in_buf, "st %d", &movetime);
      continue;
    }

    if (!strcmp(command, "level")) {
      sec = 0;
      movetime = -1;
      if (sscanf(in_buf, "level %d %d %d", &mps, &time_left, &inc) != 3) {
        sscanf(in_buf, "level %d %d %d", &mps, &time_left, &inc);
        printf("[DEBUG] level with :\n");
      } else {
        printf("[DEBUG] level without :\n");
      }

      time_left *= 60000;
      time_left += sec * 1000;
      movestogo[0] = movestogo[1] = 30;
      if (mps != 0) {
        movestogo[0] = movestogo[1] = mps;
      }

      time = -1;
      printf("[DEBUG] level timeleft: %d movestogo: %d inc: %d mps: %d",
             time_left, movestogo[0], inc, mps);
      continue;
    }

    if (!strcmp(command, "ping")) {
      printf("pong%s\n", in_buf + 4);
      continue;
    }

    if (!strcmp(command, "new")) {
      engine_side = BLACK;
      parse_fen(START_FEN, pos);
      depth = -1;
      continue;
    }

    if (!strcmp(command, "new")) {
      engine_side = BOTH;
      parse_fen(in_buf + 9, pos);
      continue;
    }

    if (!strcmp(command, "go")) {
      engine_side = pos->turnColor;
      continue;
    }

    if (!strcmp(command, "usermove")) {
      movestogo[pos->turnColor]--;
      move = parse_move(pos, in_buf + 9);
      if (move == NOMOVE) {
        continue;
      }
      make_move(pos, move);
      pos->ply = 0;
    }
  }
}

void console_loop(S_BOARD *pos, S_SEARCHINFO *info) {
  printf("Welcome to cengine in console mode!\n");
  printf("Type help for commands\n\n");

  info->game_mode = CONSOLE;
  info->post_thinking = TRUE;
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

  int depth = MAX_DEPTH;
  int movetime = 3000;
  int engineside = BOTH;

  int move = NOMOVE;
  char inbuf[80];
  char command[80];

  engineside = BLACK;
  parse_fen(START_FEN, pos);

  while (TRUE) {
    fflush(stdout);

    if (pos->turnColor == engineside && check_result(pos) == FALSE) {
      info->starttime = get_time_ms();
      info->depth = depth;

      if (movetime != 0) {
        info->timeset = TRUE;
        info->stoptime = info->starttime + movetime;
      }

      search_position(pos, info);
    }

    printf("\ncengine $ ");
    fflush(stdout);

    memset(&inbuf[0], 0, sizeof(inbuf));
    fflush(stdout);

    if (!fgets(inbuf, 80, stdin)) {
      continue;
    }

    sscanf(inbuf, "%s", command);

    if (!strcmp(command, "help")) {
      printf("Commands:\n");
      printf(" quit - quit game\n");
      printf(" force - computer will not think\n");
      printf(" print - show board\n");
      printf(" post - show thinking\n");
      printf(" nopost - do not show thinking\n");
      printf(" new - start new game\n");
      printf(" go - set computer thinking\n");
      printf(" depth x - set depth to x\n");
      printf(" time x - set thinking time to x seconds (depth still applies if "
             "set)\n");
      printf(" view - show current depth and movetime settings\n");
      printf(" setboard x - set position to fen x\n");
      printf("  ** note ** - to reset time and depth, set to 0\n");
      printf(" enter moves using b7b8q notation\n\n\n");
      continue;
    }

    if (!strcmp(command, "quit")) {
      info->quit = TRUE;
      break;
    }

    if (!strcmp(command, "post")) {
      info->post_thinking = TRUE;
      continue;
    }

    if (!strcmp(command, "print")) {
      print_board(pos);
      continue;
    }

    if (!strcmp(command, "nopost")) {
      info->post_thinking = FALSE;
      continue;
    }

    if (!strcmp(command, "force")) {
      engineside = BOTH;
      continue;
    }
    if (!strcmp(command, "view")) {
      if (depth == MAX_DEPTH) {
        printf("depth not set ");
      } else {
        printf("depth %d", depth);
      }

      if (movetime != 0) {
        printf(" movetime %ds\n", movetime / 1000);
      } else {
        printf(" movetime not set\n");
      }

      continue;
    }

    if (!strcmp(command, "depth")) {
      sscanf(inbuf, "depth %d", &depth);
      if (depth == 0) {
        depth = MAX_DEPTH;
      }
      continue;
    }

    if (!strcmp(command, "time")) {
      sscanf(inbuf, "time %d", &movetime);
      movetime *= 1000;
      continue;
    }

    if (!strcmp(command, "new")) {
      engineside = BLACK;
      parse_fen(START_FEN, pos);
      continue;
    }

    if (!strcmp(command, "go")) {
      engineside = pos->turnColor;
      continue;
    }

    move = parse_move(pos, inbuf);
    if (move == NOMOVE) {
      printf("Command unknown:%s\n", inbuf);
      continue;
    }
    make_move(pos, move);
    pos->ply = 0;
  }
}
