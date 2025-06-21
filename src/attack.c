#include "../include/defs.h"
#include <stdio.h>

const int knight_dir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int rook_dir[4] = {-1, -10, 1, 10};
const int bishop_dir[4] = {-9, -11, 11, 9};
const int king_dir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

int square_attacked(const int sq, const int side, const S_BOARD *pos) {
  ASSERT(is_on_board(sq));
  ASSERT(is_turn_color_valid(side));
  ASSERT(check_board(pos));

  int piece, idx, t_sq, dir;

  // is square of interest being attacked by pawn
  if (side == WHITE) {
    // diagnol "above"
    if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) {
      return TRUE;
    }
  } else {
    // diagnol "below"
    if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP) {
      return TRUE;
    }
  }

  // is square of interest being attacked by knight
  for (idx = 0; idx < 8; ++idx) {
    piece = pos->pieces[sq + knight_dir[idx]];
    if (is_kn(piece) && PieceCol[piece] == side) {
      return TRUE;
    }
  }

  // is square of interest being attacked by rook/queen
  for (idx = 0; idx < 4; ++idx) {
    dir = rook_dir[idx];
    t_sq = sq + dir;
    piece = pos->pieces[t_sq];

    while (piece != OFFBOARD) {
      if (piece != EMPTY) {
        if (is_rq(piece) && PieceCol[piece] == side) {
          return TRUE;
        }
        break;
      }
      t_sq += dir;
      piece = pos->pieces[t_sq];
    }
  }

  // is square of interest being attacked by bishop/queen
  for (idx = 0; idx < 4; ++idx) {
    dir = bishop_dir[idx];
    t_sq = sq + dir;
    piece = pos->pieces[t_sq];

    while (piece != OFFBOARD) {
      if (piece != EMPTY) {
        if (is_bq(piece) && PieceCol[piece] == side) {
          return TRUE;
        }
        break;
      }
      t_sq += dir;
      piece = pos->pieces[t_sq];
    }
  }

  // is square of interest being attacked by king
  for (idx = 0; idx < 8; ++idx) {
    piece = pos->pieces[sq + king_dir[idx]];

    if (is_king(piece) && PieceCol[piece] == side) {
      return TRUE;
    }
  }

  return FALSE;
}
