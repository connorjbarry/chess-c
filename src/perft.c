#include "../include/defs.h"
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

long leaf_nodes;

void perf_t(S_BOARD *pos, int depth) {
  ASSERT(check_board(pos));

  if (depth == 0) {
    leaf_nodes++;
    return;
  }

  S_MOVELIST m_list[1];
  generate_all_moves(pos, m_list);

  int move_num = 0;

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    if (!make_move(pos, m_list->moves[move_num].move)) {
      continue;
    }

    perf_t(pos, depth - 1);
    undo_move(pos);
  }

  return;
}

void perft_test(S_BOARD *pos, int depth) {
  ASSERT(check_board(pos));
  print_board(pos);

  printf("\nStarting test to depth: %d\n", depth);

  leaf_nodes = 0;

  // start timing
  struct timeval start, end;
  gettimeofday(&start, NULL);

  S_MOVELIST m_list[1];
  generate_all_moves(pos, m_list);

  int move = 0;
  int move_num = 0;

  for (move_num = 0; move_num < m_list->count; ++move_num) {
    move = m_list->moves[move_num].move;

    if (!make_move(pos, move)) {
      continue;
    }

    long cum_nodes = leaf_nodes;

    // Start timing for this move
    struct timeval move_start, move_end;
    gettimeofday(&move_start, NULL);

    perf_t(pos, depth - 1);

    // End timing for this move
    gettimeofday(&move_end, NULL);

    undo_move(pos);

    long old_nodes = leaf_nodes - cum_nodes;

    double move_time = (move_end.tv_sec - move_start.tv_sec) +
                       (move_end.tv_usec - move_start.tv_usec) / 1e6;

    printf("move %d :: %s :: %ld :: %.3f s\n", move_num + 1,
           move_to_string(move), old_nodes, move_time);
  }

  // end timing
  gettimeofday(&end, NULL);

  double elapsed_time =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

  char node_str[32];
  _format_number_with_commas(leaf_nodes, node_str);
  printf("\nTest complete: %s nodes visited.\n", node_str);
  printf("Time taken: %.3f seconds\n", elapsed_time);

  return;
}

void _format_number_with_commas(long number, char *out_str) {
  char buffer[32];
  sprintf(buffer, "%ld", number);

  int len = strlen(buffer);
  int commas = (len - 1) / 3;
  int new_len = len + commas;

  out_str[new_len] = '\0';

  int i = len - 1;
  int j = new_len - 1;
  int count = 0;

  while (i >= 0) {
    if (count == 3) {
      out_str[j--] = ',';
      count = 0;
    }
    out_str[j--] = buffer[i--];
    count++;
  }
}
