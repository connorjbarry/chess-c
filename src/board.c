#include "../include/defs.h"
#include <stdio.h>

S_BOARD *gen_board(void) {
  S_BOARD *board = (S_BOARD *)malloc(sizeof(S_BOARD));
  board->pvTable->pTable = NULL;
  init_pvtable(board->pvTable);
  return board;
}

int check_board(const S_BOARD *pos) {
  int t_pieceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int t_nonPawnPieces[2] = {0, 0};
  int t_majorPieces[2] = {0, 0};
  int t_minorPieces[2] = {0, 0};
  int t_material[2] = {0, 0};

  int sq64, t_piece, t_piece_num, sq120, color, p_count;

  U64 t_pawns[3] = {0ULL, 0ULL, 0ULL};

  t_pawns[WHITE] = pos->pawns[WHITE];
  t_pawns[BLACK] = pos->pawns[BLACK];
  t_pawns[BOTH] = pos->pawns[BOTH];

  // check piece lists
  for (t_piece = wP; t_piece <= bK; ++t_piece) {
    for (t_piece_num = 0; t_piece_num < pos->pieceNum[t_piece]; ++t_piece_num) {
      sq120 = pos->pieceList[t_piece][t_piece_num];
      ASSERT(pos->pieces[sq120] == t_piece);
    }
  }

  // check piece count and other counters
  for (sq64 = 0; sq64 < 64; ++sq64) {
    sq120 = to_sq_120(sq64);
    t_piece = pos->pieces[sq120];
    t_pieceNum[t_piece]++;
    color = PieceCol[t_piece];

    if (PieceNotPawn[t_piece] == TRUE) {
      t_nonPawnPieces[color]++;
    }
    if (PieceMajor[t_piece] == TRUE) {
      t_majorPieces[color]++;
    }
    if (PieceMinor[t_piece] == TRUE) {
      t_minorPieces[color]++;
    }

    t_material[color] += PieceVal[t_piece];
  }

  for (t_piece = wP; t_piece <= bK; ++t_piece) {
    ASSERT(t_pieceNum[t_piece] == pos->pieceNum[t_piece]);
  }

  // check bitboard counts
  p_count = count(t_pawns[WHITE]);
  ASSERT(p_count == pos->pieceNum[wP]);
  p_count = count(t_pawns[BLACK]);
  ASSERT(p_count == pos->pieceNum[bP]);
  p_count = count(t_pawns[BOTH]);
  ASSERT(p_count == (pos->pieceNum[bP] + pos->pieceNum[wP]));

  // check bitboard squares
  while (t_pawns[WHITE]) {
    sq64 = pop(&t_pawns[WHITE]);
    ASSERT(pos->pieces[to_sq_120(sq64)] == wP);
  }
  while (t_pawns[BLACK]) {
    sq64 = pop(&t_pawns[BLACK]);
    ASSERT(pos->pieces[to_sq_120(sq64)] == bP);
  }
  while (t_pawns[BOTH]) {
    sq64 = pop(&t_pawns[BOTH]);
    ASSERT((pos->pieces[to_sq_120(sq64)] == wP) ||
           (pos->pieces[to_sq_120(sq64)] == bP));
  }

  ASSERT((t_material[WHITE] == pos->material[WHITE]) &&
         (t_material[BLACK] == pos->material[BLACK]));

  ASSERT((t_minorPieces[WHITE] == pos->minorPieces[WHITE]) &&
         (t_minorPieces[BLACK] == pos->minorPieces[BLACK]));

  ASSERT((t_majorPieces[WHITE] == pos->majorPieces[WHITE]) &&
         (t_majorPieces[BLACK] == pos->majorPieces[BLACK]));

  ASSERT((t_nonPawnPieces[WHITE] == pos->nonPawnPieces[WHITE]) &&
         (t_nonPawnPieces[BLACK] == pos->nonPawnPieces[BLACK]));

  ASSERT(pos->turnColor == WHITE || pos->turnColor == BLACK);
  if (generate_pos_key(pos) != (pos->posKey)) {
    printf("FAILED POSKEY! Expected: %llx Got: %llx\n", pos->posKey,
           generate_pos_key(pos));
    print_board(pos);
  }
  ASSERT(generate_pos_key(pos) == pos->posKey);

  ASSERT(pos->enPassant == NO_SQ ||
         (RanksBrd[pos->enPassant] == RANK_6 && pos->turnColor == WHITE) ||
         (RanksBrd[pos->enPassant] == RANK_3 && pos->turnColor == BLACK));
  ASSERT(pos->pieces[pos->kingSqs[WHITE]] == wK);
  ASSERT(pos->pieces[pos->kingSqs[BLACK]] == bK);

  return TRUE;
}

void update_list_material(S_BOARD *pos) {
  int piece, sq, idx;

  for (idx = 0; idx < BOARD_NUM_SQ; ++idx) {
    sq = idx;
    piece = pos->pieces[idx];

    if (piece != OFFBOARD && piece != EMPTY) {
      int color = PieceCol[piece];
      ASSERT(is_turn_color_valid(color))
      if (PieceNotPawn[piece] == TRUE) {
        pos->nonPawnPieces[color]++;
      }
      if (PieceMajor[piece] == TRUE) {
        pos->majorPieces[color]++;
      }
      if (PieceMinor[piece] == TRUE) {
        pos->minorPieces[color]++;
      }

      pos->material[color] += PieceVal[piece];

      // piece list
      pos->pieceList[piece][pos->pieceNum[piece]] = sq;
      pos->pieceNum[piece]++;

      if (piece == wK) {
        pos->kingSqs[WHITE] = sq;
      }
      if (piece == bK) {
        pos->kingSqs[BLACK] = sq;
      }

      if (piece == wP) {
        set_bit(pos->pawns[WHITE], to_sq_64(sq));
        set_bit(pos->pawns[BOTH], to_sq_64(sq));
      } else if (piece == bP) {
        set_bit(pos->pawns[BLACK], to_sq_64(sq));
        set_bit(pos->pawns[BOTH], to_sq_64(sq));
      }
    }
  }
}

int parse_fen(char *fen, S_BOARD *pos) {
  ASSERT(fen != NULL);
  ASSERT(pos != NULL);

  int rank = RANK_8;
  int file = FILE_A;
  int piece = 0;
  int cnt = 0;
  int idx = 0;
  int sq_64 = 0;
  int sq_120 = 0;

  reset_board(pos);

  while ((rank >= RANK_1) && *fen) {
    cnt = 1;
    switch (*fen) {
    case 'p':
      piece = bP;
      break;
    case 'r':
      piece = bR;
      break;
    case 'n':
      piece = bN;
      break;
    case 'b':
      piece = bB;
      break;
    case 'k':
      piece = bK;
      break;
    case 'q':
      piece = bQ;
      break;

    case 'P':
      piece = wP;
      break;
    case 'R':
      piece = wR;
      break;
    case 'N':
      piece = wN;
      break;
    case 'B':
      piece = wB;
      break;
    case 'K':
      piece = wK;
      break;
    case 'Q':
      piece = wQ;
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      piece = EMPTY;
      cnt = *fen - '0';
      break;

    case '/':
    case ' ':
      rank--;
      file = FILE_A;
      fen++;
      continue;

    default:
      printf("FEN error \n");
      printf("%c\n", *fen);
      return -1;
    }

    for (idx = 0; idx < cnt; idx++) {
      sq_64 = rank * 8 + file;
      sq_120 = to_sq_120(sq_64); // idk what function should be, potential bug
      if (piece != EMPTY) {
        pos->pieces[sq_120] = piece;
      }
      file++;
    }

    fen++;
  }

  ASSERT(*fen == 'w' || *fen == 'b');

  pos->turnColor = (*fen == 'w') ? WHITE : BLACK;
  fen += 2;

  for (idx = 0; idx < 4; idx++) {
    if (*fen == ' ') {
      break;
    }
    switch (*fen) {
    case 'K':
      pos->castlePermission |= WKCA;
      break;
    case 'Q':
      pos->castlePermission |= WQCA;
      break;
    case 'k':
      pos->castlePermission |= BKCA;
      break;
    case 'q':
      pos->castlePermission |= BQCA;
      break;
    default:
      break;
    }
    fen++;
  }

  fen++;

  ASSERT(pos->castlePermission >= 0 && pos->castlePermission <= 15)

  if (*fen != '-') {
    file = fen[0] - 'a';
    rank = fen[1] - '1';

    ASSERT(file >= FILE_A && file <= FILE_H);
    ASSERT(rank >= RANK_1 && rank <= RANK_8);

    pos->enPassant = square_to_index(file, rank);
  }

  pos->posKey = generate_pos_key(pos);

  update_list_material(pos);

  return 0;
}

void reset_board(S_BOARD *pos) {
  int idx = 0;

  for (idx = 0; idx < BOARD_NUM_SQ; ++idx) {
    pos->pieces[idx] = OFFBOARD;
  }

  for (idx = 0; idx < 64; ++idx) {
    pos->pieces[to_sq_120(idx)] = EMPTY;
  }

  for (idx = 0; idx < 2; ++idx) {
    pos->nonPawnPieces[idx] = 0;
    pos->majorPieces[idx] = 0;
    pos->minorPieces[idx] = 0;
    pos->material[idx] = 0;
  }

  for (idx = 0; idx < 3; ++idx) {
    pos->pawns[idx] = 0ULL;
  }

  for (idx = 0; idx < 13; ++idx) {
    pos->pieceNum[idx] = 0;
  }

  pos->kingSqs[WHITE] = pos->kingSqs[BLACK] = NO_SQ;

  pos->turnColor = BOTH;
  pos->enPassant = NO_SQ;
  pos->fiftyMoveCtr = 0;

  pos->ply = 0;
  pos->historyPly = 0;

  pos->castlePermission = 0;

  pos->posKey = 0ULL;

  /*
  pos->pvTable->pTable = NULL;

  init_pvtable(pos->pvTable);
  */
}

void print_board(const S_BOARD *pos) {
  int square, file, rank, piece;

  printf("\n-- GAME BOARD --\n\n");

  for (rank = RANK_8; rank >= RANK_1; rank--) {
    printf("%d", rank + 1);

    for (file = FILE_A; file <= FILE_H; file++) {
      square = square_to_index(file, rank);
      piece = pos->pieces[square];
      printf("%3c", PieceChar[piece]);
    }

    printf("\n");
  }

  printf("\n ");

  for (file = FILE_A; file <= FILE_H; file++) {
    printf("%3c", 'a' + file);
  }

  printf("\n\n");
  printf("side: %c\n", SideChar[pos->turnColor]);
  printf("enPassant %d\n", pos->enPassant);

  printf("castle: %c%c%c%c\n", pos->castlePermission & WKCA ? 'K' : '-',
         pos->castlePermission & WQCA ? 'Q' : '-',
         pos->castlePermission & BKCA ? 'k' : '-',
         pos->castlePermission & BQCA ? 'q' : '-');

  printf("Postition key: %llX\n", pos->posKey);
}
