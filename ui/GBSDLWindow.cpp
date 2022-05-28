// GBSDLWindow.cpp
#ifdef WITH_SDL

#include "GBSDLWindow.h"
#include "GBErrors.h"
#include "GBStringUtilities.h"
#include "GBSDLApplication.h"

GBSDLWindow::GBSDLWindow(std::shared_ptr<GBView> contents, bool vis, bool is_main, GBFontManager& fontmgr)
	: visible(vis)
  , bounds(0, 0, contents->PreferredWidth(), contents->PreferredHeight())
  , sdlwindow(contents->Name(), SDL::WindowPosUndefined, SDL::WindowPosUndefined, contents->PreferredWidth(), contents->PreferredHeight(), SDL::Window::Shown | (contents->Resizable() ? SDL::Window::Resizable : 0))
  , renderer(sdlwindow, -1, 0)
  , graphics(renderer.renderer, &fontmgr)
  , view(contents)
	, isMain(is_main)
{
  #ifdef __EMSCRIPTEN__
  SDL_Rect r = SDL::GetDisplayUsableBounds(0);
	view->SetSize(r.w - 1, r.h - 84);
  sdlwindow.SetSize(r.w - 1, r.h - 84);
  #else
  view->SetSize(contents->PreferredWidth(), contents->PreferredHeight());
  #endif
	windowid = sdlwindow.GetID();
	if (visible && !isMain) Show();
	else  Update(false);
}

GBSDLWindow::~GBSDLWindow() {}

void GBSDLWindow::Update(bool running) {
	view->DoDraw(graphics, running);
  renderer.Present();
}

bool GBSDLWindow::DrawChanges(bool running) {
	bool redrawn = visible && view->NeedsRedraw(running);
	if (redrawn) {
		view->DoDraw(graphics, running);
    renderer.Present();
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
    sdlwindow.SetSize(view->PreferredWidth(), view->PreferredHeight());
  }
	this->Update(false);
}

void GBSDLWindow::SetFocus(bool focus) {view->SetFocus(focus);}

Uint32 GBSDLWindow::WindowID() const { return windowid; }
const GBRect & GBSDLWindow::Bounds() const { return bounds; };
bool GBSDLWindow::GetFrontClicks() const { return view->GetFrontClicks(); }
void GBSDLWindow::AcceptClick(int x, int y, short clicksBefore) { view->DoClick(x - bounds.left, y - bounds.top, clicksBefore); }
void GBSDLWindow::AcceptDrag(int x, int y) { view->DoDrag(x - bounds.left, y - bounds.top); }
void GBSDLWindow::AcceptKeystroke(char what) { view->AcceptKeystroke(what); }
void GBSDLWindow::AcceptUnclick(int x, int y, short clicksBefore) { view->DoUnclick(x - bounds.left, y - bounds.top, clicksBefore); }
#endif
