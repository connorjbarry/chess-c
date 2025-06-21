#include "../include/defs.h"
#include <stdio.h>

U64 generate_pos_key(const S_BOARD *pos) {
  int sq = 0;
  U64 finalKey = 0;
  int piece = EMPTY;

  for (sq = 0; sq < BOARD_NUM_SQ; ++sq) {
    piece = pos->pieces[sq];
    if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
      ASSERT(piece >= wP && piece <= bK);
      finalKey ^= PieceKeys[piece][sq];
    }
  }

  if (pos->turnColor == WHITE) {
    finalKey ^= SideKey;
  }

  if (pos->enPassant != NO_SQ) {
    ASSERT(pos->enPassant >= 0 && pos->enPassant < BOARD_NUM_SQ);
    finalKey ^= PieceKeys[EMPTY][pos->enPassant];
  }

  ASSERT(pos->castlePermission >= 0 && pos->castlePermission <= 15);

  finalKey ^= CastleKeys[pos->castlePermission];

  return finalKey;
}
