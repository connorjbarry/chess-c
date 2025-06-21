#include "../include/defs.h"

#define hash_piece(piece, sq) (pos->posKey ^= (PieceKeys[(piece)][(sq)]))
#define hash_ca (pos->posKey ^= (CastleKeys[(pos->castlePermission)]))
#define hash_side (pos->posKey ^= (SideKey))
#define hash_ep (pos->posKey ^= (PieceKeys[EMPTY][(pos->enPassant)]))

const int castle_perm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 13, 15, 15, 15, 12, 15, 15, 14, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 7,  15, 15, 15, 3,  15, 15, 11, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15};

static void clear_piece(S_BOARD *pos, const int sq) {
  ASSERT(is_on_board(sq));
  ASSERT(check_board(pos));

  int piece = pos->pieces[sq];

  ASSERT(is_piece_valid(piece));

  int color = PieceCol[piece];
  int idx = 0;
  int t_piece_num = -1;

  ASSERT(is_turn_color_valid(color));

  hash_piece(piece, sq);

  pos->pieces[sq] = EMPTY;
  pos->material[color] -= PieceVal[piece];

  if (PieceNotPawn[piece]) {
    pos->nonPawnPieces[color]--;

    if (PieceMajor[piece]) {
      pos->majorPieces[color]--;
    } else {
      pos->minorPieces[color]--;
    }
  } else {
    clear_bit(pos->pawns[color], to_sq_64(sq));
    clear_bit(pos->pawns[BOTH], to_sq_64(sq));
  }

  for (idx = 0; idx < pos->pieceNum[piece]; ++idx) {
    if (pos->pieceList[piece][idx] == sq) {
      t_piece_num = idx;
      break;
    }
  }

  ASSERT(t_piece_num != -1);
  ASSERT(t_piece_num >= 0 && t_piece_num <= 10);

  pos->pieceNum[piece]--;
  pos->pieceList[piece][t_piece_num] =
      pos->pieceList[piece][pos->pieceNum[piece]];
}

static void add_piece(S_BOARD *pos, const int sq, const int piece) {
  ASSERT(check_board(pos));
  ASSERT(is_piece_valid(piece));
  ASSERT(is_on_board(sq));

  int color = PieceCol[piece];
  ASSERT(is_turn_color_valid(color));

  hash_piece(piece, sq);

  pos->pieces[sq] = piece;

  if (PieceNotPawn[piece]) {
    pos->nonPawnPieces[color]++;

    if (PieceMajor[piece]) {
      pos->majorPieces[color]++;
    } else {
      pos->minorPieces[color]++;
    }
  } else {
    set_bit(pos->pawns[color], to_sq_64(sq));
    set_bit(pos->pawns[BOTH], to_sq_64(sq));
  }

  pos->material[color] += PieceVal[piece];

  pos->pieceList[piece][pos->pieceNum[piece]++] = sq;
}

static void move_piece(S_BOARD *pos, const int from, const int to) {
  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));

  int idx = 0;
  int piece = pos->pieces[from];
  int color = PieceCol[piece];

#ifdef DEBUG
  int t_piece_num = FALSE;
#endif

  hash_piece(piece, from);
  pos->pieces[from] = EMPTY;

  hash_piece(piece, to);
  pos->pieces[to] = piece;

  if (!PieceNotPawn[piece]) {
    clear_bit(pos->pawns[color], to_sq_64(from));
    clear_bit(pos->pawns[BOTH], to_sq_64(from));
    set_bit(pos->pawns[color], to_sq_64(to));
    set_bit(pos->pawns[BOTH], to_sq_64(to));
  }

  for (idx = 0; idx < pos->pieceNum[piece]; ++idx) {
    if (pos->pieceList[piece][idx] == from) {
      pos->pieceList[piece][idx] = to;

#ifdef DEBUG
      t_piece_num = TRUE;
#endif
      break;
    }
  }

  ASSERT(t_piece_num);
}

int make_move(S_BOARD *pos, int move) {
  ASSERT(check_board(pos));

  int from = move_from_sq(move);
  int to = move_to_sq(move);
  int side = pos->turnColor;

  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));
  ASSERT(is_turn_color_valid(side));
  ASSERT(is_piece_valid(pos->pieces[from]));

  pos->history[pos->historyPly].posKey = pos->posKey;

  if (move & MFLAG_EP) {
    if (side == WHITE) {
      clear_piece(pos, to - 10);
    } else {
      clear_piece(pos, to + 10);
    }
  } else if (move & MFLAG_CA) {
    switch (to) {
    case C1:
      move_piece(pos, A1, D1);
      break;
    case C8:
      move_piece(pos, A8, D8);
      break;
    case G1:
      move_piece(pos, H1, F1);
      break;
    case G8:
      move_piece(pos, H8, F8);
      break;
    default:
      ASSERT(FALSE);
      break;
    }
  }

  if (pos->enPassant != NO_SQ) {
    hash_ep;
  }
  hash_ca;

  pos->history[pos->historyPly].move = move;
  pos->history[pos->historyPly].fiftyMoveCtr = pos->fiftyMoveCtr;
  pos->history[pos->historyPly].enPassant = pos->enPassant;
  pos->history[pos->historyPly].castlePermission = pos->castlePermission;

  pos->castlePermission &= castle_perm[from];
  pos->castlePermission &= castle_perm[to];
  pos->enPassant = NO_SQ;

  hash_ca;

  int captured_move = capture_move(move);
  pos->fiftyMoveCtr++;

  if (captured_move != EMPTY) {
    ASSERT(is_piece_valid(captured_move));
    clear_piece(pos, to);
    pos->fiftyMoveCtr = 0;
  }

  pos->historyPly++;
  pos->ply++;

  if (PiecePawn[pos->pieces[from]]) {
    pos->fiftyMoveCtr = 0;

    if (move & MFLAG_PS) {
      if (side == WHITE) {
        pos->enPassant = from + 10;
        ASSERT(RanksBrd[pos->enPassant] == RANK_3);
      } else {
        pos->enPassant = from - 10;
        ASSERT(RanksBrd[pos->enPassant] == RANK_6);
      }
      hash_ep;
    }
  }

  move_piece(pos, from, to);

  int promotion_piece = promotion_move(move);

  if (promotion_piece != EMPTY) {
    ASSERT(is_piece_valid(promotion_piece) && !PiecePawn[promotion_piece]);
    clear_piece(pos, to);
    add_piece(pos, to, promotion_piece);
  }

  if (PieceKing[pos->pieces[to]]) {
    pos->kingSqs[pos->turnColor] = to;
  }

  pos->turnColor ^= 1;
  hash_side;

  ASSERT(check_board(pos));

  if (square_attacked(pos->kingSqs[side], pos->turnColor, pos)) {
    undo_move(pos);
    return FALSE;
  }

  return TRUE;
}

void undo_move(S_BOARD *pos) {
  ASSERT(check_board(pos));

  pos->historyPly--;
  pos->ply--;

  int move = pos->history[pos->historyPly].move;
  int from = move_from_sq(move);
  int to = move_to_sq(move);

  ASSERT(is_on_board(from));
  ASSERT(is_on_board(to));

  if (pos->enPassant != NO_SQ) {
    hash_ep;
  }
  hash_ca;

  pos->castlePermission = pos->history[pos->historyPly].castlePermission;
  pos->fiftyMoveCtr = pos->history[pos->historyPly].fiftyMoveCtr;
  pos->enPassant = pos->history[pos->historyPly].enPassant;

  if (pos->enPassant != NO_SQ) {
    hash_ep;
  }
  hash_ca;

  pos->turnColor ^= 1;
  hash_side;

  if (MFLAG_EP & move) {
    if (pos->turnColor == WHITE) {
      add_piece(pos, to - 10, bP);
    } else {
      add_piece(pos, to + 10, wP);
    }
  } else if (MFLAG_CA & move) {
    switch (to) {
    case C1:
      move_piece(pos, D1, A1);
      break;
    case C8:
      move_piece(pos, D8, A8);
      break;
    case G1:
      move_piece(pos, F1, H1);
      break;
    case G8:
      move_piece(pos, F8, H8);
      break;
    default:
      ASSERT(FALSE);
      break;
    }
  }

  move_piece(pos, to, from);

  if (PieceKing[pos->pieces[from]]) {
    pos->kingSqs[pos->turnColor] = from;
  }

  int captured_piece = capture_move(move);
  if (captured_piece != EMPTY) {
    ASSERT(is_piece_valid(captured_piece));
    add_piece(pos, to, captured_piece);
  }

  int promoted_piece = promotion_move(move);
  if (promoted_piece != EMPTY) {
    ASSERT(is_piece_valid(promoted_piece) && !PiecePawn[promoted_piece]);
    clear_piece(pos, from);
    add_piece(pos, from, PieceCol[promoted_piece] == WHITE ? wP : bP);
  }

  ASSERT(check_board(pos));
}
