// GBFontManager.cpp

#ifdef WITH_SDL
#include "GBFontManager.h"
#include "GBErrors.h"

#define FONTNAME "Vera.ttf"
#define FONTNAME_BOLD "VeraBd.ttf"
GBFontManager::GBFontManager() : fontlist() {
	if (!TTF_WasInit()) {
		if (TTF_Init() == -1) {
			FatalError("TTF_Init failure: " + string(TTF_GetError()));
		} else {
			atexit(TTF_Quit);
		}
	}
}
GBFontManager::~GBFontManager() {}
GBFontManager::GBTTFFont::GBTTFFont(uint8_t ptsize, bool bold) : ptsize(ptsize), bold(bold) {
	TTF_Font* font = TTF_OpenFont(bold ? FONTNAME_BOLD : FONTNAME, ptsize);
	if (font == nil) {
		FatalError("TTF_OpenFont failure: " + string(TTF_GetError()));
	}
  this->font = font;
}
GBFontManager::GBTTFFont::~GBTTFFont() {
  TTF_CloseFont(font);
}

TTF_Font* GBFontManager::loadFont(uint8_t ptsize, bool bold) {
	return fontlist.emplace_back(ptsize, bold).font;
}
TTF_Font* GBFontManager::findFont(uint8_t ptsize, bool bold) {
  for (auto const& font : fontlist) {
		if (font.ptsize == ptsize && font.bold == bold) return font.font;
	}
	return loadFont(ptsize, bold); // else load it
}

void GBRenderedText::draw(SDL_Renderer* r, const SDL_Rect* src, const SDL_Rect* dst) {
  SDL_RenderCopy(r, t, src, dst);
}

GBRenderedText GBFontManager::renderText_Blended(SDL_Renderer* renderer, uint8_t ptsize, bool bold, const std::string & text, SDL_Color fgcol) {
	SDL_Surface* textsurface = TTF_RenderText_Blended(findFont(ptsize, bold), text.c_str(), fgcol);
	if (textsurface == nil) FatalError("TTF_RenderText_Blended failure: " + string(TTF_GetError()));

  SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, textsurface);
  uint16_t w = textsurface->w;
  uint16_t h = textsurface->h;
	SDL_FreeSurface(textsurface);
  return GBRenderedText(w, h, t);
}
#endif
