// GBFontManager.h

#ifndef _GBFontManager_h
#define _GBFontManager_h

#include "GBPlatform.h"

#include <SDL_ttf.h>
#include <list>

class GBFontManager {
	struct GBTTFFont {
		TTF_Font* font;
		int ptsize;
	};
	std::list<GBTTFFont> fontlist;
	TTF_Font* loadFont(int ptsize);
	TTF_Font* findFont(int ptsize);
public:
	GBFontManager();
	~GBFontManager();
	SDL_Surface* renderText_Blended(int ptsize, const char* text, SDL_Color fgcol);
};

#endif
