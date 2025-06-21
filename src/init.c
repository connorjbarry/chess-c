#include "../include/defs.h"
#include "stdlib.h"
#include <stdio.h>

#define rand_64                                                                \
  ((U64)rand() + ((U64)rand() << 15) + ((U64)rand() << 30) +                   \
   ((U64)rand() << 45) + (((U64)rand() & 0xf) << 60))

int Sq120ToSq64[BOARD_NUM_SQ];
int Sq64ToSq120[64];

U64 SetMask[64];
U64 ClearMask[64];

U64 PieceKeys[13][120];
U64 SideKey;
U64 CastleKeys[16];

int FilesBrd[BOARD_NUM_SQ];
int RanksBrd[BOARD_NUM_SQ];

void init_files_ranks(void) {
  int idx = 0;
  int file = FILE_A;
  int rank = RANK_1;
  int sq = A1;

  for (idx = 0; idx < BOARD_NUM_SQ; ++idx) {
    FilesBrd[idx] = OFFBOARD;
    RanksBrd[idx] = OFFBOARD;
  }

  for (rank = RANK_1; rank <= RANK_8; rank++) {
    for (file = FILE_A; file <= FILE_H; file++) {
      sq = square_to_index(file, rank);
      FilesBrd[sq] = file;
      RanksBrd[sq] = rank;
    }
  }
}

void init_hash_keys(void) {
  int idx = 0;
  int indx = 0;

  for (idx = 0; idx < 13; ++idx) {
    for (indx = 0; indx < 120; ++indx) {
      PieceKeys[idx][indx] = rand_64;
    }
  }

  SideKey = rand_64;

  for (idx = 0; idx < 16; ++idx) {
    CastleKeys[idx] = rand_64;
  }
}

void init_bit_masks(void) {
  int idx = 0;

  for (idx = 0; idx < 64; idx++) {
    SetMask[idx] = 0ULL;
    ClearMask[idx] = 0ULL;
  }

  for (idx = 0; idx < 64; idx++) {
    SetMask[idx] |= (1ULL << idx);
    ClearMask[idx] = ~SetMask[idx];
  }
}

void init_board_base_arrays(void) {
  int idx = 0;
  int file = FILE_A;
  int rank = RANK_1;
  int square = A1;
  int square64 = 0;

  for (idx = 0; idx < BOARD_NUM_SQ; ++idx) {
    Sq120ToSq64[idx] = 65;
  }

  for (idx = 0; idx < 64; ++idx) {
    Sq64ToSq120[idx] = 120;
  }

  for (rank = RANK_1; rank <= RANK_8; ++rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      square = square_to_index(file, rank);
      Sq64ToSq120[square64] = square;
      Sq120ToSq64[square] = square64;
      square64++;
    }
  }
}

void init(void) {
  init_board_base_arrays();
  init_bit_masks();
  init_hash_keys();
  init_files_ranks();
  init_mvv_lva();
}
