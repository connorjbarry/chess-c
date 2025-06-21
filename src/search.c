#include "../include/defs.h"

#define CHECK_INTERVAL 2048

/* Check if timer is up, or interrupt triggered */
static void check_in(S_SEARCHINFO *info) {
  if (info->timeset == TRUE && get_time_ms() > info->stoptime) {
    info->stopped = TRUE;
  }

  read_input(info);
}

static void pick_next_move(S_MOVELIST *m_list, int move_num) {
  S_MOVE temp;
  int idx = 0;
  int best_score = 0;
  int best_num = move_num;

  for (idx = move_num; idx < m_list->count; ++idx) {
    if (m_list->moves[idx].score > best_score) {
      best_score = m_list->moves[idx].score;
      best_num = idx;
    }
  }

  temp = m_list->moves[move_num];
  m_list->moves[move_num] = m_list->moves[best_num];
  m_list->moves[best_num] = temp;
}

int is_repetition(const S_BOARD *pos) {
  int idx = 0;

  for (idx = pos->historyPly - pos->fiftyMoveCtr; idx < pos->historyPly - 1;
       ++idx) {
    ASSERT(idx >= 0 && idx <= MAX_GAME_MOVES);

    if (pos->posKey == pos->history[idx].posKey) {
      return TRUE;
    }
  }
  return FALSE;
}

static void clear_for_search(S_BOARD *pos, S_SEARCHINFO *info) {
  int idx = 0;
  int indx = 0;

  for (idx = 0; idx < 13; ++idx) {
    for (indx = 0; indx < BOARD_NUM_SQ; ++indx) {
      pos->searchHistory[idx][indx] = 0;
    }
  }

  for (idx = 0; idx < 2; ++idx) {
    for (indx = 0; indx < MAX_DEPTH; ++indx) {
      pos->searchKillers[idx][indx] = 0;
    }
  }

  clear_pvtable(pos->pvTable);
  pos->ply = 0;

  info->stopped = 0;
  info->nodes = 0;

  info->fh = 0;
  info->fhf = 0;
}

static int quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
  ASSERT(check_board(pos));

  if ((info->nodes & (CHECK_INTERVAL - 1)) == 0) {
    check_in(info);
  }

  info->nodes++;

  if (is_repetition(pos) || pos->fiftyMoveCtr >= 100) {
    return 0;
  }

  int score = eval_pos(pos);

  if (score >= beta) {
    return beta;
  }

  if (score > alpha) {
    alpha = score;
  }

  S_MOVELIST m_list[1];
  generate_all_captures(pos, m_list);

  int move_num = 0;
  int legal = 0;
  int old_alpha = alpha;
  int best_move = NOMOVE;
  score = -INFINITE;
  int pv_move = probe_pvtable(pos);

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    pick_next_move(m_list, move_num);
    if (!make_move(pos, m_list->moves[move_num].move)) {
      continue;
    }

    legal++;
    score = -quiescence(-beta, -alpha, pos, info);
    undo_move(pos);

    if (info->stopped == TRUE) {
      return 0;
    }

    if (score > alpha) {
      if (score >= beta) {
        if (legal == 1) {
          info->fhf++;
        }
        info->fh++;

        return beta;
      }
      alpha = score;
      best_move = m_list->moves[move_num].move;
    }
  }

  if (alpha != old_alpha) {
    store_pvmove(pos, best_move);
  }

  return alpha;
}

static int alphabeta(int alpha, int beta, int depth, S_BOARD *pos,
                     S_SEARCHINFO *info, int do_null) {
  ASSERT(check_board(pos));

  if (depth == 0) {
    return quiescence(alpha, beta, pos, info);
    // return eval_pos(pos);
  }

  if ((info->nodes & 2047) == 0) {
    check_in(info);
  }

  info->nodes++;

  if (is_repetition(pos) || pos->fiftyMoveCtr >= 100) {
    return 0;
  }

  if (pos->ply > MAX_DEPTH - 1) {
    return eval_pos(pos);
  }

  S_MOVELIST m_list[1];
  generate_all_moves(pos, m_list);

  int move_num = 0;
  int legal = 0;
  int old_alpha = alpha;
  int best_move = NOMOVE;
  int score = -INFINITE;
  int pv_move = probe_pvtable(pos);

  if (pv_move != NOMOVE) {
    for (move_num = 0; move_num < m_list->count; ++move_num) {
      if (m_list->moves[move_num].move == pv_move) {
        m_list->moves[move_num].score = 2000000;
        break;
      }
    }
  }

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    pick_next_move(m_list, move_num);
    if (!make_move(pos, m_list->moves[move_num].move)) {
      continue;
    }

    legal++;
    score = -alphabeta(-beta, -alpha, depth - 1, pos, info, TRUE);
    undo_move(pos);

    if (info->stopped == TRUE) {
      return 0;
    }

    if (score > alpha) {
      if (score >= beta) {
        if (legal == 1) {
          info->fhf++;
        }
        info->fh++;

        if (!(m_list->moves[move_num].move & MFLAG_CAP)) {
          pos->searchKillers[1][pos->ply] = pos->searchKillers[0][pos->ply];
          pos->searchKillers[0][pos->ply] = m_list->moves[move_num].move;
        }
        return beta;
      }
      alpha = score;
      best_move = m_list->moves[move_num].move;

      if (!(m_list->moves[move_num].move & MFLAG_CAP)) {
        pos->searchHistory[pos->pieces[move_from_sq(best_move)]]
                          [move_to_sq(best_move)] += depth;
      }
    }
  }

  if (legal == 0) {
    if (square_attacked(pos->kingSqs[pos->turnColor], pos->turnColor ^ 1,
                        pos)) {
      return -INFINITE + pos->ply;
    } else {
      return 0;
    }
  }

  if (alpha != old_alpha) {
    store_pvmove(pos, best_move);
  }

  return alpha;
}

void search_position(S_BOARD *pos, S_SEARCHINFO *info) {
  int best_move = NOMOVE;
  int best_score = -INFINITE;
  int current_depth = 0;
  int pv_moves = 0;
  int pv_num = 0;

  clear_for_search(pos, info);

  // iterative deepening for search
  for (current_depth = 1; current_depth <= info->depth; ++current_depth) {
    best_score = alphabeta(-INFINITE, INFINITE, current_depth, pos, info, TRUE);

    if (info->stopped == TRUE) {
      break;
    }

    pv_moves = get_pvline(pos, current_depth);
    best_move = pos->pv_arr[0];

#ifdef DEBUG
    printf("Depth: %d :: score: %d :: move: %s :: nodes: %ld ", current_depth,
           best_score, move_to_string(best_move), info->nodes);
#else
    printf("info score cp %d depth %d nodes %ld time %llu ", best_score,
           current_depth, info->nodes, get_time_ms() - info->starttime);
#endif

    pv_moves = get_pvline(pos, current_depth);

    printf("pv");
    for (pv_num = 0; pv_num < pv_moves; ++pv_num) {
      printf(" %s", move_to_string(pos->pv_arr[pv_num]));
    }
    printf("\n");
    // printf("Ordering: %.2f\n", (info->fhf / info->fh));
  }

  printf("bestmove %s\n", move_to_string(best_move));
}
