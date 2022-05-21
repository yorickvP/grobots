// GBFontManager.h

#ifndef _GBFontManager_h
#define _GBFontManager_h
#ifdef WITH_SDL
#include "GBPlatform.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <list>
class GBRenderedText;

class GBFontManager {
	struct GBTTFFont {
    GBTTFFont(uint8_t ptsize, bool bold);
    GBTTFFont(GBTTFFont&) = delete;
    ~GBTTFFont();
		TTF_Font* font;
		uint8_t ptsize;
    bool bold;
	};
	std::list<GBTTFFont> fontlist;
	TTF_Font* loadFont(uint8_t ptsize, bool bold);
	TTF_Font* findFont(uint8_t ptsize, bool bold);
public:
	GBFontManager();
	~GBFontManager();
	GBRenderedText renderText_Blended(SDL_Renderer* renderer, uint8_t ptsize, bool bold, const std::string & text, SDL_Color fgcol);
};
class GBRenderedText {
public:
  uint16_t w, h;
  SDL_Texture *t;
  void draw(SDL_Renderer* r, const SDL_Rect*, const SDL_Rect*);
  GBRenderedText(uint16_t w, uint16_t h, SDL_Texture *t) : w(w), h(h), t(t) {};
  GBRenderedText(GBRenderedText&) = delete;
  ~GBRenderedText() {
    SDL_DestroyTexture(t);
  };
};
#endif
#endif
