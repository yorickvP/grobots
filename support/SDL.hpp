#ifdef WITH_SDL
#pragma once

#include "SDL.h"

namespace SDL {
  const int WindowPosUndefined = SDL_WINDOWPOS_UNDEFINED;
  inline SDL_Rect GetDisplayUsableBounds(int displayindex) {
    SDL_Rect r;
    SDL_GetDisplayUsableBounds(displayindex, &r);
    return r;
  };
  class Window {
  public:
    SDL_Window* sdlwindow;
    Window(const std::string& title, int x, int y, int w, int h, Uint32 flags) {
      sdlwindow = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
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
    static const int PosUndefined = SDL_WINDOWPOS_UNDEFINED;
    static const Uint32 Shown = SDL_WINDOW_SHOWN;
    static const Uint32 Resizable = SDL_WINDOW_RESIZABLE;
  };
  class Renderer {
  public:
    SDL_Renderer* renderer;
    Renderer(Window& w, int index, Uint32 flags) {
      renderer = SDL_CreateRenderer(w.sdlwindow, index, flags);
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

