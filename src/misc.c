#include "../include/defs.h"
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#else
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#endif

long long get_time_ms(void) {
#ifdef WIN32
  return GetTickCount();
#else
  struct timeval t;
  gettimeofday(&t, NULL);
  return (long long)t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}

int input_waiting(void) {
#ifndef WIN32
  fd_set readfds;
  struct timeval tv;
  FD_ZERO(&readfds);
  FD_SET(fileno(stdin), &readfds);
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select(16, &readfds, 0, 0, &tv);

  return (FD_ISSET(fileno(stdin), &readfds));
#else
  static int init = 0, pipe;
  static HANDLE inh;
  DWORD dw;

  if (!init) {
    init = 1;
    inh = GetStdHandle(STD_INPUT_HANDLE);
    pipe = !GetConsoleMode(inh, &dw);
    if (!pipe) {
      SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
      FlushConsoleInputBuffer(inh);
    }
  }
  if (pipe) {
    if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
      return 1;
    return dw;
  } else {
    GetNumberOfConsoleInputEvents(inh, &dw);
    return dw <= 1 ? 0 : dw;
  }
#endif
}

void read_input(S_SEARCHINFO *info) {
  int bytes;
  char input[256] = "", *endc;

  if (input_waiting()) {
    info->stopped = TRUE;
    do {
      bytes = read(fileno(stdin), input, 256);
    } while (bytes < 0);
    endc = strchr(input, '\n');
    if (endc)
      *endc = 0;

    if (strlen(input) > 0) {
      if (!strncmp(input, "quit", 4)) {
        info->quit = TRUE;
      }
    }
    return;
  }
}
