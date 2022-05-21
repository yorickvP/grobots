// GBFontManager.h

#ifndef _GBFontManager_h
#define _GBFontManager_h
#ifdef WITH_SDL
#include "GBPlatform.h"

#include <SDL_ttf.h>
#include <list>

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
	SDL_Surface* renderText_Blended(uint8_t ptsize, bool bold, const char* text, SDL_Color fgcol);
};
#endif
#endif
