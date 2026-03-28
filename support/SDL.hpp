#ifdef WITH_SDL
#pragma once

#include <SDL3/SDL.h>

namespace SDL {
  inline SDL_Rect GetDisplayUsableBounds(int displayindex) {
    SDL_Rect r = {0, 0, 0, 0};
    SDL_GetDisplayUsableBounds((SDL_DisplayID)displayindex, &r);
    return r;
  };
  class Window {
  public:
    SDL_Window* sdlwindow;
    Window(const std::string& title, int w, int h, Uint32 flags) {
      sdlwindow = SDL_CreateWindow(title.c_str(), w, h, flags);
    };
    Window(const Window&) = delete;
    ~Window() {
      SDL_DestroyWindow(sdlwindow);
    };
    void SetSize(int w, int h) {
      SDL_SetWindowSize(sdlwindow, w, h);
    }
    Uint32 GetID() {
      return SDL_GetWindowID(sdlwindow);
    };
    static const Uint32 Shown = 0; // windows are shown by default in SDL3
    static const Uint32 Resizable = SDL_WINDOW_RESIZABLE;
  };
  class Renderer {
  public:
    SDL_Renderer* renderer;
    Renderer(Window& w, const char* driver) {
      renderer = SDL_CreateRenderer(w.sdlwindow, driver);
    };
    Renderer(const Renderer&) = delete;
    ~Renderer() {
      SDL_DestroyRenderer(renderer);
    };
    void Present() {
      SDL_RenderPresent(renderer);
    }
  };
}

#endif

