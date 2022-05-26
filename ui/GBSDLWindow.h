// GBWindow.h

#ifdef WITH_SDL

#ifndef GBSDLWindow_h
#define GBSDLWindow_h

#include "GBTypes.h"
#include "GBView.h"
#include "SDL.h"
#include <memory>
class GBSDLWindow {
  std::shared_ptr<GBView> view;
	bool visible;
	GBGraphics * graphics;
	SDL_Window* sdlwindow;
  SDL_Renderer* renderer;
	
	GBRect bounds;
	
	Uint32 windowid;
public:
	bool isMain;
public:
	GBSDLWindow(std::shared_ptr<GBView> contents, bool vis, bool is_main, GBFontManager& fontmgr);
	~GBSDLWindow();
	
// window operations
	void Update(bool running);
	bool DrawChanges(bool moreChancesSoon);
	void Show(); //show and bring to front
	void Hide();
	bool Visible() const;

	void AcceptClick(int x, int y, short clicksBefore);
	void AcceptDrag(int x, int y);
	void AcceptUnclick(int x, int y, short clicksBefore);
	void AcceptKeystroke(char what);
	bool GetFrontClicks() const;
//sizing
	void ResizeSelf();
	void SetSize(short width, short height);
	const GBRect & Bounds() const;
	
	Uint32 WindowID() const;
};

#endif

#endif
