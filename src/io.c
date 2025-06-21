#include "../include/defs.h"
#include <stdio.h>

char *square_to_string(const int sq) {
  static char square_str[3];

  int file = FilesBrd[sq];
  int rank = RanksBrd[sq];

  sprintf(square_str, "%c%c", ('a' + file), ('1' + rank));

  return square_str;
}

char *move_to_string(const int move) {
  static char move_str[6];

  int from_file = FilesBrd[move_from_sq(move)];
  int from_rank = RanksBrd[move_from_sq(move)];
  int to_file = FilesBrd[move_to_sq(move)];
  int to_rank = RanksBrd[move_to_sq(move)];

  int promoted = promotion_move(move);

  if (promoted) {
    char p_char = 'q';

    if (is_kn(promoted)) {
      p_char = 'n';
    } else if (is_rq(promoted) && !is_bq(promoted)) {
      p_char = 'r';
    } else if (!is_rq(promoted) && is_bq(promoted)) {
      p_char = 'b';
    }

    sprintf(move_str, "%c%c%c%c%c", ('a' + from_file), ('1' + from_rank),
            ('a' + to_file), ('1' + to_rank), p_char);
  } else {
    sprintf(move_str, "%c%c%c%c", ('a' + from_file), ('1' + from_rank),
            ('a' + to_file), ('1' + to_rank));
  }

  return move_str;
}

void move_list_to_string(const S_MOVELIST *m_list) {
  int idx = 0;
  int score = 0;
  int move = 0;

  printf("-- MoveList -- \n");

  for (idx = 0; idx < m_list->count; ++idx) {
    move = m_list->moves[idx].move;
    score = m_list->moves[idx].score;

    printf("Move: %d > %s (score: %d) \n", idx + 1, move_to_string(move),
           score);
  }

  printf("Total Moves: %d", m_list->count);
}

int parse_move(S_BOARD *pos, char *move_str) {
  if (move_str[1] > '8' || move_str[1] < '1') {
    return FALSE;
  }
  if (move_str[3] > '8' || move_str[3] < '1') {
    return FALSE;
  }
  if (move_str[0] > 'h' || move_str[0] < 'a') {
    return FALSE;
  }
  if (move_str[2] > 'h' || move_str[2] < 'a') {
    return FALSE;
  }

  int from = square_to_index(move_str[0] - 'a', move_str[1] - '1');
  int to = square_to_index(move_str[2] - 'a', move_str[3] - '1');

  ASSERT(is_on_board(from) && is_on_board(to));

  S_MOVELIST m_list[1];
  generate_all_moves(pos, m_list);

  int move_num = 0;
  int move = 0;
  int promote_piece = EMPTY;

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    move = m_list->moves[move_num].move;
    if (move_from_sq(move) == from && move_to_sq(move) == to) {
      promote_piece = promotion_move(move);

      if (promote_piece != EMPTY) {
        if (is_rq(promote_piece) && !is_bq(promote_piece) &&
            move_str[4] == 'r') {
          return move;
        } else if (!is_rq(promote_piece) && is_bq(promote_piece) &&
                   move_str[4] == 'b') {
          return move;
        } else if (is_rq(promote_piece) && is_bq(promote_piece) &&
                   move_str[4] == 'q') {
          return move;
        } else if (is_kn(promote_piece) && move_str[4] == 'n') {
          return move;
        }
        continue;
      }
      return move;
    }
  }

  return NOMOVE;
}
