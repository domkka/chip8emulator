#include "Chip8.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <array>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} App;

int main(int argc, char *argv[]) {
  Chip8 chip8;
  chip8.reset();

  if (!chip8.loadROM("roms/ibm.ch8")) {
    return 1;
  }

  App app = {0};
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  app.window = SDL_CreateWindow("Chip-8 Emulator", 640, 320, 0);
  if (!app.window) {
    SDL_Log("Could not create windows: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  app.renderer = SDL_CreateRenderer(app.window, NULL);
  if (!app.renderer) {
    SDL_Log("Could not create windows: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  int running = 1;
  const int scale = 10;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = 0;
      }
    }

    chip8.cycle();

    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);

    SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);

    for (int y = 0; y < chip8.DISPLAY_HEIGHT; ++y) {
      for (int x = 0; x < chip8.DISPLAY_WIDTH; ++x) {
        if (chip8.getDisplay()[y * 64 + x]) {
          SDL_FRect rect = {
              static_cast<float>(x * scale),
              static_cast<float>(y * scale),
              static_cast<float>(scale),
              static_cast<float>(scale)};
          SDL_RenderFillRect(app.renderer, &rect);
        }
      }
    }
    SDL_RenderPresent(app.renderer);
    SDL_Delay(16);
  }
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit;
  return 0;
}