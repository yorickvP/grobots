// GBFontManager.cpp

#include "GBFontManager.h"
#include "GBErrors.h"

#define FONTNAME "Vera.ttf"
GBFontManager::GBFontManager() : fontlist() {
	if (!TTF_WasInit()) {
		if (TTF_Init() == -1) {
			FatalError("TTF_Init failure: " + string(TTF_GetError()));
		} else {
			atexit(TTF_Quit);
		}
	}
}
GBFontManager::~GBFontManager() {
	while (!fontlist.empty()) {
		TTF_CloseFont(fontlist.back().font);
		fontlist.pop_back();
	}
}
TTF_Font* GBFontManager::loadFont(int ptsize) {
	GBTTFFont f;
	f.ptsize = ptsize;
	f.font = TTF_OpenFont(FONTNAME, ptsize);
	if (f.font == nil) {
		FatalError("TTF_OpenFont failure: " + string(TTF_GetError()));
	}
	fontlist.push_back(f);
	return f.font;
}
TTF_Font* GBFontManager::findFont(int ptsize) {
	for (std::list<GBTTFFont>::iterator it = fontlist.begin() ; it != fontlist.end(); it++) {
		if (it->ptsize == ptsize) return it->font;
	}
	return loadFont(ptsize); // else load it
}

SDL_Surface* GBFontManager::renderText_Blended(int ptsize, const char* text, SDL_Color fgcol) {
	SDL_Surface* textsurface = TTF_RenderText_Blended(findFont(ptsize), text, fgcol);
	if (textsurface == nil) FatalError("TTF_RenderText_Blended failure: " + string(TTF_GetError()));
	return textsurface;
}
