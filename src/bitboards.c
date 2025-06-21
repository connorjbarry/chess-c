#include "../include/defs.h"
#include "stdio.h"

const int bit_table[64] = {63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11,
                           53, 19, 34, 61, 29, 2,  51, 21, 43, 45, 10, 18, 47,
                           1,  54, 9,  57, 0,  35, 62, 31, 40, 4,  49, 5,  52,
                           26, 60, 6,  23, 44, 46, 27, 56, 16, 7,  39, 48, 24,
                           59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

int pop_bit(U64 *bb) {
  U64 b = *bb ^ (*bb - 1);
  unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
  *bb &= (*bb - 1);
  return bit_table[(fold * 0x783a9b23) >> 26];
}

int count_bits(U64 bb) {
  int count;
  for (count = 0; bb; count++, bb &= bb - 1)
    ;
  return count;
}

void print_bit_board(U64 bb) {
  U64 shift = 1ULL;

  int rank = 0;
  int file = 0;
  int sq = 0;
  int sq_64 = 0;

  printf("\n");

  for (rank = RANK_8; rank >= RANK_1; --rank) {
    for (file = FILE_A; file <= FILE_H; ++file) {
      sq = square_to_index(file, rank);
      sq_64 = to_sq_64(sq);

      if ((shift << sq_64) & bb) {
        printf("X");
      } else {
        printf("-");
      }
    }
    printf("\n");
  }
  printf("\n\n");
}
