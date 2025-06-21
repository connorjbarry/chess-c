#include "../include/defs.h"

int is_on_board(const int sq) { return FilesBrd[sq] != OFFBOARD ? 1 : 0; }

int is_turn_color_valid(const int side) {
  return (side == WHITE || side == BLACK) ? 1 : 0;
}

int is_file_rank_valid(const int fr) { return (fr >= 0 && fr <= 7) ? 1 : 0; }

int is_piece_valid_or_empty(const int piece) {
  return (piece >= EMPTY && piece <= bK) ? 1 : 0;
}

int is_piece_valid(const int piece) {
  return (piece >= wP && piece <= bK) ? 1 : 0;
}
