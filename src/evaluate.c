#include "../include/defs.h"

const int pawn_table[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,  10, 10, 0,  -10, -10, 0,  10, 10,
    5,  0,  0,  5,  5,  0,  0,  5,  0,  0,  10, 20,  20,  10, 0,  0,
    5,  5,  5,  10, 10, 5,  5,  5,  10, 10, 10, 20,  20,  10, 10, 10,
    20, 20, 20, 30, 30, 20, 20, 20, 0,  0,  0,  0,   0,   0,  0,  0};

const int knight_table[64] = {
    0, -10, 0,  0,  0,  0,  -10, 0, 0, 0,  0,  5,  5,  0,  0,  0,
    0, 0,   10, 10, 10, 10, 0,   0, 0, 0,  10, 20, 20, 10, 5,  0,
    5, 10,  15, 20, 20, 15, 10,  5, 5, 10, 10, 20, 20, 10, 10, 5,
    0, 0,   5,  10, 10, 5,  0,   0, 0, 0,  0,  0,  0,  0,  0,  0};

const int bishop_table[64] = {
    0, 0,  -10, 0,  0,  -10, 0,  0, 0, 0,  0,  10, 10, 0,  0,  0,
    0, 0,  10,  15, 15, 10,  0,  0, 0, 10, 15, 20, 20, 15, 10, 0,
    0, 10, 15,  20, 20, 15,  10, 0, 0, 0,  10, 15, 15, 10, 0,  0,
    0, 0,  0,   10, 10, 0,   0,  0, 0, 0,  0,  0,  0,  0,  0,  0};

const int rook_table[64] = {0,  0,  5,  10, 10, 5,  0,  0,  0,  0,  5,  10, 10,
                            5,  0,  0,  0,  0,  5,  10, 10, 5,  0,  0,  0,  0,
                            5,  10, 10, 5,  0,  0,  0,  0,  5,  10, 10, 5,  0,
                            0,  0,  0,  5,  10, 10, 5,  0,  0,  25, 25, 25, 25,
                            25, 25, 25, 25, 0,  0,  5,  10, 10, 5,  0,  0};

int eval_pos(const S_BOARD *pos) {
  int piece;
  int piece_num;
  int sq;
  int score = pos->material[WHITE] - pos->material[BLACK];

  piece = wP;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score += pawn_table[to_sq_64(sq)];
  }

  piece = bP;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score -= pawn_table[mirror(to_sq_64(sq))];
  }

  piece = wN;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score += knight_table[to_sq_64(sq)];
  }

  piece = bN;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score -= knight_table[mirror(to_sq_64(sq))];
  }

  piece = wB;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score += bishop_table[to_sq_64(sq)];
  }

  piece = bB;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score -= bishop_table[mirror(to_sq_64(sq))];
  }

  piece = wR;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score += rook_table[to_sq_64(sq)];
  }

  piece = bR;
  for (piece_num = 0; piece_num < pos->pieceNum[piece]; ++piece_num) {
    sq = pos->pieceList[piece][piece_num];
    ASSERT(is_on_board(sq));
    score -= rook_table[mirror(to_sq_64(sq))];
  }

  if (pos->turnColor == WHITE) {
    return score;
  } else {
    return -score;
  }
}
