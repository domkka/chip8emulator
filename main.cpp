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

  if (!chip8.loadROM("roms/snake.ch8")) {
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

  const double INSTRUCTIONS_PER_SECOND = 700.0;
  const double MS_PER_INSTRUCTION = 1000.0 / INSTRUCTIONS_PER_SECOND;
  uint32_t lastCycleTime = SDL_GetTicks();

  const double TIMER_HZ = 60.0;
  const double MS_PER_TIMER_TICK = 1000.0 / TIMER_HZ;
  uint32_t lastTimerUpdate = SDL_GetTicks();

  uint32_t lastDrawTime = SDL_GetTicks();

  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = 0;
      } else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = (event.type == SDL_EVENT_KEY_DOWN);

        switch (event.key.key) {
        case SDLK_1:
          chip8.setKey(0x1, pressed);
          break;
        case SDLK_2:
          chip8.setKey(0x2, pressed);
          break;
        case SDLK_3:
          chip8.setKey(0x3, pressed);
          break;
        case SDLK_4:
          chip8.setKey(0xC, pressed);
          break;

        case SDLK_Q:
          chip8.setKey(0x4, pressed);
          break;
        case SDLK_W:
          chip8.setKey(0x5, pressed);
          break;
        case SDLK_E:
          chip8.setKey(0x6, pressed);
          break;
        case SDLK_R:
          chip8.setKey(0xD, pressed);
          break;

        case SDLK_A:
          chip8.setKey(0x7, pressed);
          break;
        case SDLK_S:
          chip8.setKey(0x8, pressed);
          break;
        case SDLK_D:
          chip8.setKey(0x9, pressed);
          break;
        case SDLK_F:
          chip8.setKey(0xE, pressed);
          break;

        case SDLK_Y:
          chip8.setKey(0xA, pressed);
          break;
        case SDLK_X:
          chip8.setKey(0x0, pressed);
          break;
        case SDLK_C:
          chip8.setKey(0xB, pressed);
          break;
        case SDLK_V:
          chip8.setKey(0xF, pressed);
          break;
        }
      }
    }

    uint32_t now = SDL_GetTicks();
    double elapsed = static_cast<double>(now - lastCycleTime);
    if (elapsed >= MS_PER_INSTRUCTION) {
      chip8.cycle();
      lastCycleTime = now;
    }

    double elapsedTimer = static_cast<double>(now - lastTimerUpdate);
    if (elapsedTimer >= MS_PER_TIMER_TICK) {
      chip8.updateTimers();
      lastTimerUpdate = now;
    }

    if (now - lastDrawTime >= MS_PER_TIMER_TICK) {
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
      lastDrawTime = now;
    }
  }
  SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit;
  return 0;
}