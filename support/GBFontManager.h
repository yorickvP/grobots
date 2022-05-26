// GBFontManager.h

#ifndef _GBFontManager_h
#define _GBFontManager_h
#ifdef WITH_SDL
#include "GBPlatform.h"
#include "GBColor.h"
#include "GBCache.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <list>
#include <memory>
class GBRenderedText;

class GBFontManager {
  struct GBTTFFont;
public:
  using Text = std::shared_ptr<GBRenderedText>;
  struct TextIx {
    const SDL_Renderer* const r;
    const TTF_Font* const font;
    const std::string text;
    const GBColor fgcol;
    bool operator==(const TextIx& other) const = default;
  };
  class IxHash {
  public:
    std::size_t operator()(const TextIx &t) const {
      std::size_t seed = 0;
      seed ^= std::hash<const void*>()(t.r) + 0x9e3779b9 + (seed<<6) + (seed>>2);
      seed ^= std::hash<const void*>()(t.font) + 0x9e3779b9 + (seed<<6) + (seed>>2);
      seed ^= std::hash<std::string>()(t.text) + 0x9e3779b9 + (seed<<6) + (seed>>2);
      seed ^= std::hash<GBColor>()(t.fgcol) + 0x9e3779b9 + (seed<<6) + (seed>>2);
      return seed;
    }
  };
private:
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
  GBCache<TextIx, Text, IxHash> cache;
public:
	GBFontManager();
	~GBFontManager();
  Text renderText_Blended(SDL_Renderer* renderer, uint8_t ptsize, bool bold, const std::string & text, const GBColor& fgcol);
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
