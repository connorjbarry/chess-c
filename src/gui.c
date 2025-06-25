#include "../include/gui.h"

int init_gui(GUI_STATE *state) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL initialization failed: %s\n", SDL_GetError());
    return 0;
  }

  int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (!(IMG_Init(img_flags) & img_flags)) {
    printf("SDL_image initialization failed: %s\n", IMG_GetError());
    SDL_Quit();
    return 0;
  }

  // Create window
  state->window = SDL_CreateWindow("cengine GUI", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
                                   WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (!state->window) {
    printf("Window creation failed: %s\n", SDL_GetError());
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  state->renderer =
      SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
  if (!state->renderer) {
    printf("Renderer creation failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(state->window);
    IMG_Quit();
    SDL_Quit();
    return 0;
  }

  state->running = 1;
  return 1;
}

void cleanup_gui(GUI_STATE *state) {
  if (state->renderer) {
    SDL_DestroyRenderer(state->renderer);
  }
  if (state->window) {
    SDL_DestroyWindow(state->window);
  }
  IMG_Quit();
  SDL_Quit();
}
