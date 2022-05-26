// GBSDLApplication.cpp
// Grobots - GBSDLApplicationClass.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Grobots (non-copyright) GBSDLApplication 2010 Yorick
// Distributed under the GNU General Public License.
#ifdef WITH_SDL
#include "GBPlatform.h"
#include "GBSDLApplication.h"
#include "GBErrors.h"
#include "GBAboutBox.h"
#include "GBMiniMap.h"
#include "GBSideReader.h"

#include "GBRosterView.h"
#include "GBScoresView.h"
#include "GBRobotTypeView.h"
#include "GBDebugger.h"
#include "GBSideDebugger.h"
#include "GBTournamentView.h"
#include "GBMultiView.h"

#include "GBMenu.h"
#include <sstream>
#include <vector>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

const short kClickRange = 5;
const GBMilliseconds kSlowerSpeedLimit = 500;
const GBMilliseconds kSlowSpeedLimit = 100;
const GBMilliseconds kNormalSpeedLimit = 33;
const GBMilliseconds kFastSpeedLimit = 17;
const GBMilliseconds kFasterSpeedLimit = 10;
const GBMilliseconds kNoSpeedLimit = 0;

const int kMaxFasterSteps = 3;
const GBMilliseconds kMaxEventInterval = 50;

GBSDLApplication::GBSDLApplication() 
	: alive(true), clicks(0), clickx(0), clicky(0), stepPeriod(-1), lastStep(0), fontmanager(),
    dragging(), world(), focus(), windows() {
  // TODO: find out which parts to init
#ifndef __EMSCRIPTEN__
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) FatalError("Unable to init SDL");
#else
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == -1) FatalError("Unable to init SDL");
#endif
	atexit(SDL_Quit);
	SDL_initFramerate(&stepManager);
	SetStepPeriod(kNormalSpeedLimit);
	
	portal = new GBPortal(world);
	mainView = std::make_shared<GBMultiView>(portal);

	mainWnd = std::make_shared<GBSDLWindow>(mainView, true, true, fontmanager);
  mainView->Add(std::make_shared<GBMenuView>(world, *this), 0, -17);
// #ifndef __EMSCRIPTEN__
// 	windows.push_back(std::make_shared<GBSDLWindow>(new GBMenuView(world, *this), true, this, false, &fontmanager));
// #endif
	focus = mainWnd;
	windows.push_back(mainWnd);
  OpenMinimap();
  OpenRoster();
	//windows.push_back(menuWnd);
	
	GBSide * side = GBSideReader::Load("the-lunacy.gb");
	if (side) world.AddSide(side);
	side = GBSideReader::Load("commune-plus-2.gb");
	if (side) world.AddSide(side);
	side = GBSideReader::Load("the-horde.gb");
	if (side) world.AddSide(side);
	world.AddSeeds();
	world.running = true;
}
GBSDLApplication::~GBSDLApplication() {}

void GBSDLApplication::mainloop(void* arg) {
  GBSDLApplication *app = static_cast<GBSDLApplication*>(arg);
  app->Process();
  app->Redraw();
	SDL_Event event;
  while (app->alive && SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_QUIT:
      app->Quit();
      return;
      break;
    default:
      app->HandleEvent(&event);
    }
  }
}

void GBSDLApplication::Run() {
  #if __EMSCRIPTEN__
  emscripten_set_main_loop_arg(GBSDLApplication::mainloop, this, -1, 1);
  #else
	do {
    GBSDLApplication::mainloop(this);
		SDL_framerateDelay(&stepManager);
	} while (alive);
  #endif
}

void GBSDLApplication::SetStepPeriod(int period) {
	stepPeriod = period;
	SDL_setFramerate(&stepManager, 1000/period);
}

void GBSDLApplication::Quit() {
	alive = false;
}

void GBSDLApplication::Process() {
	lastStep = Milliseconds();
	if ( !world.running ) {
		lastStep += 1000; //hack to prevent taking so much time when paused at Unlimited speed
		return;
	}
	try {
		int steps = 0;
		do {
			world.AdvanceFrame();
			++steps;
		} while ( world.running && (stepPeriod <= 0 || (stepPeriod <= 10 && steps < kMaxFasterSteps))
			&& Milliseconds() <= lastStep + kMaxEventInterval );
	} catch ( GBError & err ) {
		NonfatalError("Error simulating: " + err.ToString());
	} catch ( GBAbort & ) {
		world.running = false;
	}
}
void GBSDLApplication::Redraw() {
	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (!(*it)->Visible()) continue;
		(*it)->DrawChanges(world.running);
	}
}

Ref<GBSDLWindow> GBSDLApplication::FindWndAtPos(short x, short y) {
	for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
		if (!(*it)->Visible()) continue;
		const GBRect &r = (*it)->Bounds();
		if (r.left <= x && r.right >= x && r.top <= y && r.bottom >= y) return *it;
	}
	return nil;
}

Ref<GBSDLWindow> GBSDLApplication::FindWndFromID(Uint32 id) {
	for (auto it = windows.rbegin(); it != windows.rend(); ++it) {
		if (!(*it)->Visible()) continue;
		if (id == (*it)->WindowID()) return *it;
	}
	return {};
}
void GBSDLApplication::HandleEvent(SDL_Event* evt) {
	try {
		switch ( evt->type ) {
			case SDL_MOUSEBUTTONDOWN:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					ExpireClicks(evt->button.x, evt->button.y);
					clickTime = Milliseconds();
					clickx = evt->button.x; clicky = evt->button.y;
					Ref<GBSDLWindow> wnd = FindWndFromID(evt->button.windowID);
					if (!wnd) break;
					if (focus.lock() != wnd && wnd != menuWnd) { // stop the menu window from getting focus (it doesn't need that)
						focus = wnd;
						//if (!wnd->GetFrontClicks()) break;
					}
					dragging = wnd;
					wnd->AcceptClick(evt->button.x, evt->button.y, clicks);
					++clicks;
				}
				if (evt->button.button == SDL_BUTTON_MIDDLE) {
					Ref<GBSDLWindow> wnd = FindWndFromID(evt->button.windowID);
					if (!wnd || wnd == mainWnd) break;
					dragging = wnd;
				}
				if (evt->button.button == SDL_BUTTON_RIGHT) {
					Ref<GBSDLWindow> wnd = FindWndFromID(evt->button.windowID);
          if (!wnd) break;
          if (wnd == mainWnd) {
            mainView->RightClick(evt->button.x, evt->button.y);
            break;
          }
					CloseWindow(wnd);
					if (dragging.lock() == wnd) dragging = {};
					if (focus.lock() == wnd) focus = {};
				}
				break;
    case SDL_MOUSEWHEEL: {
      Ref<GBSDLWindow> wnd = FindWndFromID(evt->wheel.windowID);
      if (!wnd) break;
      if (evt->wheel.y > 0) {
        wnd->AcceptKeystroke('+');
      } else if (evt->wheel.y < 0) {
        wnd->AcceptKeystroke('-');
      }
      // todo: smooth zooming using preciseY
    } break;
			case SDL_MOUSEBUTTONUP:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					if ( auto d = dragging.lock() ) {
						ExpireClicks(evt->button.x, evt->button.y);
						d->AcceptUnclick(evt->button.x, evt->button.y, clicks);
						dragging = {};
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if (evt->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					if (auto d = dragging.lock() ) {
						d->AcceptDrag(evt->motion.x, evt->motion.y);
					}
				}
				break;
			case SDL_KEYDOWN:
				if (auto f = focus.lock()) {
          // FIXME: deal with modifiers
          f->AcceptKeystroke(evt->key.keysym.sym);
				}
				break;
    case SDL_WINDOWEVENT: {
		        Ref<GBSDLWindow> wnd = FindWndFromID(evt->window.windowID);
				if (!wnd) break;
				switch (evt->window.event) {
				    case SDL_WINDOWEVENT_SHOWN:
				    case SDL_WINDOWEVENT_EXPOSED:
				        wnd->Update(false);
				        break;
				    case SDL_WINDOWEVENT_RESIZED:
				        wnd->SetSize(evt->window.data1, evt->window.data2);
				        break;
				    case SDL_WINDOWEVENT_CLOSE:
				        this->CloseWindow(wnd);
				        break;
				}
				break; }
			//case SDL_VIDEORESIZE:
			//	main_screen = SDL_SetVideoMode(evt->resize.w, evt->resize.h, 32, SDL_SWSURFACE | SDL_RESIZABLE);
			//	mainGrf->setSurface(main_screen);
			//	mainWnd->SetSize(evt->resize.w, evt->resize.h);
			default:
				break;
		}
	} catch ( GBError & err ) {
		NonfatalError("Error processing event: " + err.ToString());
	}
}
void GBSDLApplication::ExpireClicks(int x, int y) {
	if ( clicks && (Milliseconds() > clickTime +
			1000
			|| abs(x - clickx) > kClickRange
			|| abs(y - clicky) > kClickRange) )
		clicks = 0;
}
void GBSDLApplication::CloseWindow(Ref<GBSDLWindow> window) {
	window->Hide();
	windows.remove(window);
}
void GBSDLApplication::OpenMinimap() {
  mainView->Add(std::make_shared<GBMiniMapView>(world, *portal), 7, mainView->Bounds().bottom - 200);
}
void GBSDLApplication::OpenDebugger() {
	mainView->Add(std::make_shared<GBDebuggerView>(world), 616, 43);
}
void GBSDLApplication::OpenAbout() {
  // TODO: center
  short x = mainView->Bounds().CenterX()-131;
  short y = mainView->Bounds().CenterY() - 118;
	mainView->Add(std::make_shared<GBAboutBox>(), x, y);
}
void GBSDLApplication::OpenSideDebugger() {
	mainView->Add(std::make_shared<GBSideDebuggerView>(world), 200, 400);
}
void GBSDLApplication::OpenRoster() {
	mainView->Add(std::make_shared<GBRosterView>(world), 7, 43);
}
void GBSDLApplication::OpenScores() {
	mainView->Add(std::make_shared<GBScoresView>(world), 291, 384);
}
void GBSDLApplication::OpenTypeWindow() {
	mainView->Add(std::make_shared<GBRobotTypeView>(world), 616, 270);
}
void GBSDLApplication::OpenTournament() {
	mainView->Add(std::make_shared<GBTournamentView>(world), 100, 100);
}

#endif
