// GBSDLWindow.cpp
#ifdef WITH_SDL

#include "GBSDLWindow.h"
#include "GBErrors.h"
#include "GBStringUtilities.h"
#include "GBSDLApplication.h"

GBSDLWindow::GBSDLWindow(GBView * contents, bool vis, GBSDLApplication * _app, bool is_main, GBFontManager* fontmgr)
	: view(contents), visible(vis), bounds(0, 0, view->PreferredWidth(), view->PreferredHeight())
	, isMain(is_main)
	, app(_app)
{
    sdlwindow = SDL_CreateWindow(contents->Name().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                            contents->PreferredWidth(), contents->PreferredHeight(), 
                                 SDL_WINDOW_SHOWN | (contents->Resizable() ? SDL_WINDOW_RESIZABLE : 0));
    renderer = SDL_CreateRenderer(sdlwindow, -1, 0);
    graphics = new GBGraphics(renderer, fontmgr);
	view->SetGraphics(graphics);
  #ifdef __EMSCRIPTEN__
  SDL_Rect r;
  SDL_GetDisplayUsableBounds(0, &r);
	view->SetSize(r.w - 1, r.h - 84);
  SDL_SetWindowSize(sdlwindow, r.w - 1, r.h - 84);
  #else
  view->SetSize(v->PreferredWidth(), v->PreferredHeight());
  #endif
	windowid = SDL_GetWindowID(sdlwindow);
	if (visible && !isMain) Show();
	else  Update(false);
}

GBSDLWindow::~GBSDLWindow() {
	if (view) delete view;
	if (graphics) delete graphics;
	if (sdlwindow) SDL_DestroyWindow(sdlwindow);
}

void GBSDLWindow::Update(bool running) {
	view->DoDraw(running);
  SDL_RenderPresent(renderer);
}

bool GBSDLWindow::DrawChanges(bool running) {
	bool redrawn = visible && view->NeedsRedraw(running);
	if (redrawn) {
		view->DoDraw(running);
    SDL_RenderPresent(renderer);
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
  if (view->NeedsResize()) {
    SDL_SetWindowSize(sdlwindow, view->PreferredWidth(), view->PreferredHeight());
  }
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
