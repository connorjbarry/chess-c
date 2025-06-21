#include "../include/defs.h"
#include <stdio.h>

#define move(from, to, ca, pro, flag)                                          \
  ((from) | ((to) << 7) | ((ca) << 14) | ((pro) << 20) | (flag))

int loop_slide_piece[8] = {wB, wR, wQ, 0, bB, bR, bQ, 0};

int loop_non_slide_piece[6] = {wN, wK, 0, bN, bK, 0};

int loop_slide_idx[2] = {0, 4};
int loop_non_slide_idx[2] = {0, 3};

const int piece_dir[13][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},           {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12}, {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},       {-1, -10, 1, 10, -9, -11, 11, 9},
    {-1, -10, 1, 10, -9, -11, 11, 9},   {0, 0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12}, {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},       {-1, -10, 1, 10, -9, -11, 11, 9},
    {-1, -10, 1, 10, -9, -11, 11, 9}};

const int num_dir[13] = {0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

const int victim_score[13] = {0,   100, 200, 300, 400, 500, 600,
                              100, 200, 300, 400, 500, 600};
static int mvv_lva_scores[13][13];

void init_mvv_lva(void) {
  int attacker;
  int victim;

  for (attacker = wP; attacker <= bK; ++attacker) {
    for (victim = wP; victim <= bK; ++victim) {
      mvv_lva_scores[victim][attacker] =
          victim_score[victim] + 6 - (victim_score[attacker] / 100);
    }
  }
}

int move_exists(S_BOARD *pos, const int move) {
  S_MOVELIST m_list[1];
  generate_all_moves(pos, m_list);

  int move_num = 0;

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    if (!make_move(pos, m_list->moves[move_num].move)) {
      continue;
    }
    undo_move(pos);

    if (m_list->moves[move_num].move == move) {
      return TRUE;
    }
  }

  return FALSE;
}

void add_quiet_move(const S_BOARD *pos, int move, S_MOVELIST *m_list) {
  ASSERT(is_on_board(move_from_sq(move)));
  ASSERT(is_on_board(move_to_sq(move)));

  m_list->moves[m_list->count].move = move;

  if (pos->searchKillers[0][pos->ply] == move) {
    m_list->moves[m_list->count].score = 900000;
  } else if (pos->searchKillers[1][pos->ply] == move) {
    m_list->moves[m_list->count].score = 800000;
  } else {
    m_list->moves[m_list->count].score =
        pos->searchHistory[pos->pieces[move_from_sq(move)]]
                          [pos->pieces[move_to_sq(move)]];
  }

  m_list->count++;
}

void add_capture_move(const S_BOARD *pos, int move, S_MOVELIST *m_list) {
  ASSERT(is_on_board(move_from_sq(move)));
  ASSERT(is_on_board(move_to_sq(move)));
  ASSERT(is_piece_valid(capture_move(move)));

  m_list->moves[m_list->count].move = move;
  m_list->moves[m_list->count].score =
      mvv_lva_scores[capture_move(move)][pos->pieces[move_from_sq(move)]] +
      1000000;
  m_list->count++;
}

void add_en_passant_move(const S_BOARD *pos, int move, S_MOVELIST *m_list) {
  ASSERT(is_on_board(move_from_sq(move)));
  ASSERT(is_on_board(move_to_sq(move)));

  m_list->moves[m_list->count].move = move;
  m_list->moves[m_list->count].score = 105 + 1000000;
  m_list->count++;
}

void add_white_pawn_cap_move(const S_BOARD *pos, const int from, const int to,
                             const int cap, S_MOVELIST *m_list) {
  ASSERT(is_piece_valid_or_empty(cap));
  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));
  ASSERT(check_board(pos));

  if (RanksBrd[from] == RANK_7) {
    add_capture_move(pos, move(from, to, cap, wQ, 0), m_list);
    add_capture_move(pos, move(from, to, cap, wB, 0), m_list);
    add_capture_move(pos, move(from, to, cap, wN, 0), m_list);
    add_capture_move(pos, move(from, to, cap, wR, 0), m_list);
  } else {
    add_capture_move(pos, move(from, to, cap, EMPTY, 0), m_list);
  }
}

void add_white_pawn_move(const S_BOARD *pos, const int from, const int to,
                         S_MOVELIST *m_list) {
  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));
  ASSERT(check_board(pos));

  if (RanksBrd[from] == RANK_7) {
    add_quiet_move(pos, move(from, to, EMPTY, wQ, 0), m_list);
    add_quiet_move(pos, move(from, to, EMPTY, wB, 0), m_list);
    add_quiet_move(pos, move(from, to, EMPTY, wN, 0), m_list);
    add_quiet_move(pos, move(from, to, EMPTY, wR, 0), m_list);
  } else {
    add_quiet_move(pos, move(from, to, EMPTY, EMPTY, 0), m_list);
  }
}

void add_black_pawn_cap_move(const S_BOARD *pos, const int from, const int to,
                             const int cap, S_MOVELIST *m_list) {
  ASSERT(is_piece_valid_or_empty(cap));
  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));
  ASSERT(check_board(pos));

  if (RanksBrd[from] == RANK_2) {
    add_capture_move(pos, move(from, to, cap, bQ, 0), m_list);
    add_capture_move(pos, move(from, to, cap, bB, 0), m_list);
    add_capture_move(pos, move(from, to, cap, bN, 0), m_list);
    add_capture_move(pos, move(from, to, cap, bR, 0), m_list);
  } else {
    add_capture_move(pos, move(from, to, cap, EMPTY, 0), m_list);
  }
}

void add_black_pawn_move(const S_BOARD *pos, const int from, const int to,
                         S_MOVELIST *m_list) {
  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));
  ASSERT(check_board(pos));

  if (RanksBrd[from] == RANK_2) {
    add_quiet_move(pos, move(from, to, EMPTY, bQ, 0), m_list);
    add_quiet_move(pos, move(from, to, EMPTY, bB, 0), m_list);
    add_quiet_move(pos, move(from, to, EMPTY, bN, 0), m_list);
    add_quiet_move(pos, move(from, to, EMPTY, bR, 0), m_list);
  } else {
    add_quiet_move(pos, move(from, to, EMPTY, EMPTY, 0), m_list);
  }
}

void generate_pawn_moves(const S_BOARD *pos, S_MOVELIST *m_list, int opp_side) {
  int piece_num = 0;
  int sq = 0;
  int pawn_type = opp_side == BLACK ? wP : bP;
  int opp_pawn_type = opp_side == BLACK ? bP : wP;
  int op = opp_side == BLACK ? 1 : -1;
  int rank = opp_side == BLACK ? RANK_2 : RANK_7;

  void (*pawn_move)(const S_BOARD *, const int, const int, S_MOVELIST *) =
      opp_side == BLACK ? add_white_pawn_move : add_black_pawn_move;

  void (*pawn_cap_move)(const S_BOARD *, const int, const int, const int,
                        S_MOVELIST *) =
      opp_side == BLACK ? add_white_pawn_cap_move : add_black_pawn_cap_move;

  for (piece_num = 0; piece_num < pos->pieceNum[pawn_type]; ++piece_num) {
    sq = pos->pieceList[pawn_type][piece_num];
    ASSERT(is_on_board(sq));

    if (pos->pieces[sq + 10 * op] == EMPTY) {
      pawn_move(pos, sq, sq + 10 * op, m_list);
      if (RanksBrd[sq] == rank && pos->pieces[sq + 20 * op] == EMPTY) {
        add_quiet_move(pos, move(sq, (sq + 20 * op), EMPTY, EMPTY, MFLAG_PS),
                       m_list);
      }
    }
    if (is_on_board(sq + 9 * op) &&
        PieceCol[pos->pieces[sq + 9 * op]] == opp_side) {
      pawn_cap_move(pos, sq, sq + 9 * op, pos->pieces[sq + 9 * op], m_list);
    }
    if (is_on_board(sq + 11 * op) &&
        PieceCol[pos->pieces[sq + 11 * op]] == opp_side) {
      pawn_cap_move(pos, sq, sq + 11 * op, pos->pieces[sq + 11 * op], m_list);
    }
    if (pos->enPassant != NO_SQ) {
      if (sq + 9 * op == pos->enPassant) {
        add_en_passant_move(pos, move(sq, sq + 9 * op, EMPTY, EMPTY, MFLAG_EP),
                            m_list);
      }
      if (sq + 11 * op == pos->enPassant) {
        add_en_passant_move(pos, move(sq, sq + 11 * op, EMPTY, EMPTY, MFLAG_EP),
                            m_list);
      }
    }
  }
}

void generate_pawn_captures(const S_BOARD *pos, S_MOVELIST *m_list,
                            int opp_side) {
  int piece_num = 0;
  int sq = 0;
  int pawn_type = opp_side == BLACK ? wP : bP;
  int opp_pawn_type = opp_side == BLACK ? bP : wP;
  int op = opp_side == BLACK ? 1 : -1;
  int rank = opp_side == BLACK ? RANK_2 : RANK_7;

  void (*pawn_cap_move)(const S_BOARD *, const int, const int, const int,
                        S_MOVELIST *) =
      opp_side == BLACK ? add_white_pawn_cap_move : add_black_pawn_cap_move;

  for (piece_num = 0; piece_num < pos->pieceNum[pawn_type]; ++piece_num) {
    sq = pos->pieceList[pawn_type][piece_num];
    ASSERT(is_on_board(sq));

    if (is_on_board(sq + 9 * op) &&
        PieceCol[pos->pieces[sq + 9 * op]] == opp_side) {
      pawn_cap_move(pos, sq, sq + 9 * op, pos->pieces[sq + 9 * op], m_list);
    }
    if (is_on_board(sq + 11 * op) &&
        PieceCol[pos->pieces[sq + 11 * op]] == opp_side) {
      pawn_cap_move(pos, sq, sq + 11 * op, pos->pieces[sq + 11 * op], m_list);
    }
    if (pos->enPassant != NO_SQ) {
      if (sq + 9 * op == pos->enPassant) {
        add_en_passant_move(pos, move(sq, sq + 9 * op, EMPTY, EMPTY, MFLAG_EP),
                            m_list);
      }
      if (sq + 11 * op == pos->enPassant) {
        add_en_passant_move(pos, move(sq, sq + 11 * op, EMPTY, EMPTY, MFLAG_EP),
                            m_list);
      }
    }
  }
}

void generate_castle_moves(const S_BOARD *pos, S_MOVELIST *m_list) {
  if (pos->turnColor == WHITE) {

    if (pos->castlePermission & WKCA) {
      if (pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY) {
        if (!square_attacked(E1, BLACK, pos) &&
            !square_attacked(F1, BLACK, pos)) {
          add_quiet_move(pos, move(E1, G1, EMPTY, EMPTY, MFLAG_CA), m_list);
        }
      }
    }
    if (pos->castlePermission & WQCA) {
      if (pos->pieces[D1] == EMPTY && pos->pieces[C1] == EMPTY &&
          pos->pieces[B1] == EMPTY) {
        if (!square_attacked(E1, BLACK, pos) &&
            !square_attacked(D1, BLACK, pos)) {
          add_quiet_move(pos, move(E1, C1, EMPTY, EMPTY, MFLAG_CA), m_list);
        }
      }
    }

  } else {
    if (pos->castlePermission & BKCA) {
      if (pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY) {
        if (!square_attacked(E8, WHITE, pos) &&
            !square_attacked(F8, WHITE, pos)) {
          add_quiet_move(pos, move(E8, G8, EMPTY, EMPTY, MFLAG_CA), m_list);
        }
      }
    }
    if (pos->castlePermission & BQCA) {
      if (pos->pieces[D8] == EMPTY && pos->pieces[C8] == EMPTY &&
          pos->pieces[B8] == EMPTY) {
        if (!square_attacked(E8, WHITE, pos) &&
            !square_attacked(D8, WHITE, pos)) {
          add_quiet_move(pos, move(E8, C8, EMPTY, EMPTY, MFLAG_CA), m_list);
        }
      }
    }
  }
}

void generate_all_moves(const S_BOARD *pos, S_MOVELIST *m_list) {
  ASSERT(check_board(pos));

  m_list->count = 0;

  int piece = EMPTY;
  int side = pos->turnColor;
  int sq = 0;
  int t_sq = 0;
  int piece_num = 0;
  int dir = 0;
  int idx = 0;
  int piece_idx = 0;

  // WHITE ^ 1 == BLACK | BLACK ^ 1 == WHITE
  generate_pawn_moves(pos, m_list, side ^ 1);

  /* Castle moves */

  generate_castle_moves(pos, m_list);

  /* Loop for slide pieces */
  piece_idx = loop_slide_idx[side];
  piece = loop_slide_piece[piece_idx++];

  while (piece != 0) {
    ASSERT(is_piece_valid(piece));

    for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
      sq = pos->pieceList[piece][piece_num];
      ASSERT(is_on_board(sq));

      for (idx = 0; idx < num_dir[piece]; ++idx) {
        dir = piece_dir[piece][idx];
        t_sq = sq + dir;

        while (is_on_board(t_sq)) {
          if (pos->pieces[t_sq] != EMPTY) {
            if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
              add_capture_move(pos, move(sq, t_sq, pos->pieces[t_sq], EMPTY, 0),
                               m_list);
            }
            break;
          }
          add_quiet_move(pos, move(sq, t_sq, EMPTY, EMPTY, 0), m_list);
          t_sq += dir;
        }
      }
    }
    piece = loop_slide_piece[piece_idx++];
  }

  /* Loop for non slide pieces */
  piece_idx = loop_non_slide_idx[side];
  piece = loop_non_slide_piece[piece_idx++];

  while (piece != 0) {
    ASSERT(is_piece_valid(piece));

    for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
      sq = pos->pieceList[piece][piece_num];
      ASSERT(is_on_board(sq));

      for (idx = 0; idx < num_dir[piece]; ++idx) {
        dir = piece_dir[piece][idx];
        t_sq = sq + dir;

        if (!is_on_board(t_sq)) {
          continue;
        }

        if (pos->pieces[t_sq] != EMPTY) {
          if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
            add_capture_move(pos, move(sq, t_sq, pos->pieces[t_sq], EMPTY, 0),
                             m_list);
          }
          continue;
        }
        add_quiet_move(pos, move(sq, t_sq, EMPTY, EMPTY, 0), m_list);
      }
    }

    piece = loop_non_slide_piece[piece_idx++];
  }
}

void generate_all_captures(const S_BOARD *pos, S_MOVELIST *m_list) {
  ASSERT(check_board(pos));

  m_list->count = 0;

  int piece = EMPTY;
  int side = pos->turnColor;
  int sq = 0;
  int t_sq = 0;
  int piece_num = 0;
  int dir = 0;
  int idx = 0;
  int piece_idx = 0;

  // WHITE ^ 1 == BLACK | BLACK ^ 1 == WHITE
  generate_pawn_captures(pos, m_list, side ^ 1);

  /* Loop for slide pieces */
  piece_idx = loop_slide_idx[side];
  piece = loop_slide_piece[piece_idx++];

  while (piece != 0) {
    ASSERT(is_piece_valid(piece));

    for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
      sq = pos->pieceList[piece][piece_num];
      ASSERT(is_on_board(sq));

      for (idx = 0; idx < num_dir[piece]; ++idx) {
        dir = piece_dir[piece][idx];
        t_sq = sq + dir;

        while (is_on_board(t_sq)) {
          if (pos->pieces[t_sq] != EMPTY) {
            if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
              add_capture_move(pos, move(sq, t_sq, pos->pieces[t_sq], EMPTY, 0),
                               m_list);
            }
            break;
          }
          t_sq += dir;
        }
      }
    }
    piece = loop_slide_piece[piece_idx++];
  }

  /* Loop for non slide pieces */
  piece_idx = loop_non_slide_idx[side];
  piece = loop_non_slide_piece[piece_idx++];

  while (piece != 0) {
    ASSERT(is_piece_valid(piece));

    for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
      sq = pos->pieceList[piece][piece_num];
      ASSERT(is_on_board(sq));

      for (idx = 0; idx < num_dir[piece]; ++idx) {
        dir = piece_dir[piece][idx];
        t_sq = sq + dir;

        if (!is_on_board(t_sq)) {
          continue;
        }

        if (pos->pieces[t_sq] != EMPTY) {
          if (PieceCol[pos->pieces[t_sq]] == (side ^ 1)) {
            add_capture_move(pos, move(sq, t_sq, pos->pieces[t_sq], EMPTY, 0),
                             m_list);
          }
          continue;
        }
      }
    }

    piece = loop_non_slide_piece[piece_idx++];
  }
}
