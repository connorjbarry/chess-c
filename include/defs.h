#ifndef DEFS_H
#define DEFS_H

#include "stdio.h"
#include "stdlib.h"
#include <sys/_types/_u_int64_t.h>

// #define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n)                                                              \
  if (!(n)) {                                                                  \
    printf("%s - Failed", #n);                                                 \
    printf("On %s ", __DATE__);                                                \
    printf("At %s ", __TIME__);                                                \
    printf("In File %s ", __FILE__);                                           \
    printf("On Line %d\n", __LINE__);                                          \
    exit(1);                                                                   \
  }
#endif

typedef unsigned long long U64;

#define NAME "cengine 0.1"
#define BOARD_NUM_SQ 120

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

// reminder: this is half moves
#define MAX_GAME_MOVES 2048
#define MAX_POSITION_MOVES 256
#define MAX_DEPTH 64

#define INFINITE 30000

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };
enum {
  FILE_A,
  FILE_B,
  FILE_C,
  FILE_D,
  FILE_E,
  FILE_F,
  FILE_G,
  FILE_H,
  FILE_NONE
};
enum {
  RANK_1,
  RANK_2,
  RANK_3,
  RANK_4,
  RANK_5,
  RANK_6,
  RANK_7,
  RANK_8,
  RANK_NONE
};
enum { WHITE, BLACK, BOTH };
enum {
  A1 = 21,
  B1,
  C1,
  D1,
  E1,
  F1,
  G1,
  H1,
  A2 = 31,
  B2,
  C2,
  D2,
  E2,
  F2,
  G2,
  H2,
  A3 = 41,
  B3,
  C3,
  D3,
  E3,
  F3,
  G3,
  H3,
  A4 = 51,
  B4,
  C4,
  D4,
  E4,
  F4,
  G4,
  H4,
  A5 = 61,
  B5,
  C5,
  D5,
  E5,
  F5,
  G5,
  H5,
  A6 = 71,
  B6,
  C6,
  D6,
  E6,
  F6,
  G6,
  H6,
  A7 = 81,
  B7,
  C7,
  D7,
  E7,
  F7,
  G7,
  H7,
  A8 = 91,
  B8,
  C8,
  D8,
  E8,
  F8,
  G8,
  H8,
  NO_SQ,
  OFFBOARD
};
enum { FALSE, TRUE };

// castling permissions
enum { WKCA = 1, WQCA = 2, BKCA = 4, BQCA = 8 };

typedef struct {
  int move;
  int score;
} S_MOVE;

typedef struct {
  S_MOVE moves[MAX_POSITION_MOVES];
  int count;
} S_MOVELIST;

typedef struct {
  U64 posKey;
  int move;
} S_PVENTRY;

typedef struct {
  S_PVENTRY *pTable;
  int numEntries;
} S_PVTABLE;

/* Information needed to undo a move */
typedef struct {
  // move itself, is in int TBD
  int move;

  int castlePermission;
  int enPassant;
  int fiftyMoveCtr;
  U64 posKey;

} S_HIST_MOVE;

typedef struct {
  int pieces[BOARD_NUM_SQ];
  U64 pawns[3];

  // what squares the kings are on
  int kingSqs[2];

  int turnColor;
  int enPassant;

  // if this hits 50 game is draw, 100 total half moves
  int fiftyMoveCtr;

  // # of half moves in current search
  int ply;

  // total # of half moves in game
  int historyPly;

  // represented as a 4 bit int
  // e.g. 1001 means black queen side castle and white king side castle
  int castlePermission;

  // unique key for each postion
  U64 posKey;

  int pieceNum[13];

  // all non pawn pieces - bigPce
  int nonPawnPieces[2];
  // Rooks and Queens - majPce
  int majorPieces[2];
  // Bishops and Knights - minPce
  int minorPieces[2];
  // material score for each side
  int material[2];

  S_HIST_MOVE history[MAX_GAME_MOVES];

  // piece list
  // i.e., pieceList[wN][0] = E1;
  int pieceList[13][10];

  S_PVTABLE pvTable[1];
  int pv_arr[MAX_DEPTH];

  // store non captures only
  int searchHistory[13][BOARD_NUM_SQ];
  int searchKillers[2][MAX_DEPTH];
} S_BOARD;

typedef struct {
  long long starttime;
  long long stoptime;
  int depth;
  int depthset;
  int timeset;
  int movestogo;
  int infinite;

  long nodes;

  int quit;
  int stopped;

  // number of times a move caused a beta cutoff
  float fh;
  // number of times first move in the list caused that cutoff
  float fhf;
} S_SEARCHINFO;

/* GAME MOVE */

// where the move is coming from
#define move_from_sq(m) ((m) & 0x7F)
// where the move is going to
#define move_to_sq(m) (((m) >> 7) & 0x7F)

#define capture_move(m) (((m) >> 14) & 0xF)
#define promotion_move(m) (((m) >> 20) & 0xF)

// flag for en passant move
#define MFLAG_EP 0x40000
// flag for pawn starting move
#define MFLAG_PS 0x80000
// flag for castle moves
#define MFLAG_CA 0x1000000

// flag for physical capture move
#define MFLAG_CAP 0x7C000
// flag for physcial promotion move
#define MFLAG_PRO 0xF00000

#define NOMOVE 0

/*  MACROS */

// converts piece file/rank to square in 120 base array
#define square_to_index(f, r) ((21 + (f)) + ((r) * 10))

// converts square to 64 base index
#define to_sq_64(sq120) (Sq120ToSq64[sq120])

// converts square to 120 base index
#define to_sq_120(sq64) (Sq64ToSq120[sq64])

#define pop(b) pop_bit(b)
#define count(b) count_bits(b)

#define clear_bit(bb, sq) ((bb) &= ClearMask[(sq)])
#define set_bit(bb, sq) ((bb) |= SetMask[(sq)])

#define is_bq(p) (PieceBishopQueen[(p)])
#define is_rq(p) (PieceRookQueen[(p)])
#define is_kn(p) (PieceKnight[(p)])
#define is_king(p) (PieceKing[(p)])

#define mirror(sq) (Mirror64[sq])

// ------------------------------------

/* GLOBALS */

// takes in a 0-119, only ever return 21-98, and provide equivalent value
// on 64 bit board
extern int Sq120ToSq64[BOARD_NUM_SQ];

// the opposite; converts a given position from 64 base array to 120 base array
extern int Sq64ToSq120[64];

extern U64 SetMask[64];
extern U64 ClearMask[64];

extern U64 PieceKeys[13][120];
extern U64 SideKey;
extern U64 CastleKeys[16];

extern char PieceChar[];
extern char SideChar[];
extern char RankChar[];
extern char FileChar[];

// all non-pawn pieces
extern int PieceNotPawn[13];
// rooks and queens
extern int PieceMajor[13];
// bishops and knights
extern int PieceMinor[13];
extern int PieceVal[13];
extern int PieceCol[13];

extern int FilesBrd[BOARD_NUM_SQ];
extern int RanksBrd[BOARD_NUM_SQ];

extern int PiecePawn[13];
extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];

extern int PieceSlides[13];

extern int Mirror64[64];
// ------------------------------------

/* FUNCTIONS */

// init.c
extern void init(void);

// bitboards.c
extern void print_bit_board(U64 bb);
extern int pop_bit(U64 *bb);
extern int count_bits(U64 bb);

// hashkeys.c
extern U64 generate_pos_key(const S_BOARD *pos);

// board.c
extern void reset_board(S_BOARD *pos);
extern int parse_fen(char *fen, S_BOARD *pos);
extern void print_board(const S_BOARD *pos);
extern void update_list_material(S_BOARD *pos);
extern int check_board(const S_BOARD *pos);
extern S_BOARD *gen_board(void);

// attack.c
extern int square_attacked(const int sq, const int side, const S_BOARD *pos);

// io.c
extern char *square_to_string(const int sq);
extern char *move_to_string(const int move);
extern void move_list_to_string(const S_MOVELIST *m_list);
extern int parse_move(S_BOARD *pos, char *move_str);

// validate.c
extern int is_on_board(const int sq);
extern int is_turn_color_valid(const int side);
extern int is_file_rank_valid(const int fr);
extern int is_piece_valid_or_empty(const int piece);
extern int is_piece_valid(const int piece);

// movegen.c
extern void generate_all_moves(const S_BOARD *pos, S_MOVELIST *m_list);
extern void generate_all_captures(const S_BOARD *pos, S_MOVELIST *m_list);
extern int move_exists(S_BOARD *pos, const int move);
extern void init_mvv_lva(void);

// move.c
extern int make_move(S_BOARD *pos, int move);
extern void undo_move(S_BOARD *pos);

// perft.c
extern void perft_test(S_BOARD *pos, int depth);
extern void _format_number_with_commas(long number, char *out_str);

// misc.c
extern long long get_time_ms(void);
extern void read_input(S_SEARCHINFO *info);

// search.c
extern void search_position(S_BOARD *pos, S_SEARCHINFO *info);

// pvt.c
extern void init_pvtable(S_PVTABLE *pvt);
extern void store_pvmove(const S_BOARD *pos, int move);
extern int probe_pvtable(const S_BOARD *pos);
extern int get_pvline(S_BOARD *pos, const int depth);
extern void clear_pvtable(S_PVTABLE *pvt);

// evaluate.c
extern int eval_pos(const S_BOARD *pos);

// uci.c
extern void uci_loop(void);

#endif
