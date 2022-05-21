// GBSDLWindow.cpp
#ifdef WITH_SDL

#include "GBSDLWindow.h"
#include "GBErrors.h"
#include "GBStringUtilities.h"
#include "GBSDLApplication.h"

GBSDLWindow::GBSDLWindow(GBView * contents, int x, int y, bool vis, GBSDLApplication * _app, bool is_main, GBFontManager* fontmgr)
	: view(contents), visible(vis), bounds(0, 0, view->PreferredWidth(), view->PreferredHeight())
	, isMain(is_main)
	, app(_app)
{
    sdlwindow = SDL_CreateWindow(contents->Name().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                            contents->PreferredWidth(), contents->PreferredHeight(), 
                                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    surf = SDL_GetWindowSurface(sdlwindow);
    graphics = new GBGraphics(surf, fontmgr);
	view->SetGraphics(graphics);
	view->SetSize(view->PreferredWidth(), view->PreferredHeight());
	windowid = SDL_GetWindowID(sdlwindow);
	if (visible && !isMain) Show();
	else  Update(false);
}

GBSDLWindow::~GBSDLWindow() {
	if (view) delete view;
	if (graphics) delete graphics;
	if (sdlwindow) SDL_DestroyWindow(sdlwindow);
}

void GBSDLWindow::Update(bool) {
	view->DoDraw();
	SDL_UpdateWindowSurface(sdlwindow);
}

bool GBSDLWindow::DrawChanges(bool running) {
	bool redrawn = visible && view->NeedsRedraw(running);
	if (redrawn) {
		view->DoDraw();
		SDL_UpdateWindowSurface(sdlwindow);
	}
	if ( !isMain && visible && view->NeedsResize() ) {
		redrawn = true;
		ResizeSelf();
	}
	return redrawn;
}
void GBSDLWindow::ResizeSelf() {
	SetSize(view->PreferredWidth(), view->PreferredHeight());
}


bool GBSDLWindow::Visible() const { return visible; }
void GBSDLWindow::Show()          { visible = true; Update(false); /* XXX: bring to front */}
void GBSDLWindow::Hide()          { visible = false; }
void GBSDLWindow::SetSize(short width, short height) {
	bounds.right = width;
	bounds.bottom = height;
	view->SetBounds(bounds);
	SDL_SetWindowSize(sdlwindow, width, height);
    surf = SDL_GetWindowSurface(sdlwindow);
    graphics->setSurface(surf);
	this->Update(false);
}

Uint32 GBSDLWindow::WindowID() const { return windowid; }
const GBRect & GBSDLWindow::Bounds() const { return bounds; };
bool GBSDLWindow::GetFrontClicks() const { return view->GetFrontClicks(); }
void GBSDLWindow::AcceptClick(int x, int y, short clicksBefore) { view->DoClick(x - bounds.left, y - bounds.top, clicksBefore); }
void GBSDLWindow::AcceptDrag(int x, int y) { view->DoDrag(x - bounds.left, y - bounds.top); }
void GBSDLWindow::AcceptKeystroke(char what) { view->AcceptKeystroke(what); }
void GBSDLWindow::AcceptUnclick(int x, int y, short clicksBefore) { view->DoUnclick(x - bounds.left, y - bounds.top, clicksBefore); }
#endif
