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

#include "GBMenu.h"
#include <sstream>
#include <vector>

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
    dragging(nil), world(), focus(nil), windows() {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) FatalError("Unable to init SDL");
	//SDL_EnableUNICODE(SDL_ENABLE);
	atexit(SDL_Quit);
	SDL_initFramerate(&stepManager);
	SetStepPeriod(kNormalSpeedLimit);
	
	portal = new GBPortal(world);
	GBView * mainView = portal;

	mainWnd = new GBSDLWindow(mainView, 0, 0, true, this, true, &fontmanager);
	windows.push_back(new GBSDLWindow(new GBMenuView(world, *this), 0, 0, true, this, false, &fontmanager));
	focus = mainWnd;
	windows.push_back(mainWnd);
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
GBSDLApplication::~GBSDLApplication() {
	while (!windows.empty()) {
		delete windows.back();
		windows.pop_back();
	}
}

void GBSDLApplication::Run() {
	SDL_Event event;
	do {
		Process();
		Redraw();
		while (alive && SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					Quit();
					return;
					break;
				default:
					HandleEvent(&event);
			}
			//Redraw();
		}
		SDL_framerateDelay(&stepManager);
	} while (alive);
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
	for (std::list<GBSDLWindow*>::iterator it = windows.begin(); it != windows.end(); ++it) {
		if (!(*it)->Visible()) continue;
		(*it)->DrawChanges(world.running);
	}
}

GBSDLWindow* GBSDLApplication::FindWndAtPos(short x, short y) {
	for (std::list<GBSDLWindow*>::reverse_iterator it = windows.rbegin(); it != windows.rend(); ++it) {
		if (!(*it)->Visible()) continue;
		const GBRect &r = (*it)->Bounds();
		if (r.left <= x && r.right >= x && r.top <= y && r.bottom >= y) return *it;
	}
	return nil;
}

GBSDLWindow* GBSDLApplication::FindWndFromID(Uint32 id) {
	for (std::list<GBSDLWindow*>::reverse_iterator it = windows.rbegin(); it != windows.rend(); ++it) {
		if (!(*it)->Visible()) continue;
		if (id == (*it)->WindowID()) return *it;
	}
	return nil;
}
void GBSDLApplication::HandleEvent(SDL_Event* evt) {
	try {
		switch ( evt->type ) {
			case SDL_MOUSEBUTTONDOWN:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					ExpireClicks(evt->button.x, evt->button.y);
					clickTime = Milliseconds();
					clickx = evt->button.x; clicky = evt->button.y;
					GBSDLWindow* wnd = FindWndFromID(evt->button.windowID);
					if (wnd == nil) break;
					if (focus != wnd && wnd != menuWnd) { // stop the menu window from getting focus (it doesn't need that)
						focus = wnd;
						//if (!wnd->GetFrontClicks()) break;
					}
					dragging = wnd;
					wnd->AcceptClick(evt->button.x, evt->button.y, clicks);
					++clicks;
				}
				if (evt->button.button == SDL_BUTTON_MIDDLE) {
					GBSDLWindow* wnd = FindWndFromID(evt->button.windowID);
					if (wnd == nil || wnd == mainWnd) break;
					dragging = wnd;
				}
				if (evt->button.button == SDL_BUTTON_RIGHT) {
					GBSDLWindow* wnd = FindWndFromID(evt->button.windowID);
					if (wnd == nil || wnd == mainWnd) break;
					CloseWindow(wnd);
					if (dragging == wnd) dragging = nil;
					if (focus == wnd) focus = mainWnd;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					if ( dragging ) {
						ExpireClicks(evt->button.x, evt->button.y);
						dragging->AcceptUnclick(evt->button.x, evt->button.y, clicks);
						dragging = nil;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if (evt->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					if ( dragging ) {
						dragging->AcceptDrag(evt->motion.x, evt->motion.y);
					}
				}
				break;
			case SDL_KEYDOWN:
				if (focus) {
          // FIXME
					//if ((evt->key.keysym.unicode & 0xFF80) == 0) {
						//focus->AcceptKeystroke(evt->key.keysym.unicode & 0x7F);
					//}
				}
				break;
		    case SDL_WINDOWEVENT: {
		        GBSDLWindow* wnd = FindWndFromID(evt->window.windowID);
				if (wnd == nil) break;
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
void GBSDLApplication::CloseWindow(GBSDLWindow* window) {
	window->Hide();
	delete window;
	windows.remove(window);
}
void GBSDLApplication::OpenMinimap() {
	windows.push_back(new GBSDLWindow(new GBMiniMapView(world, *portal), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenDebugger() {
	windows.push_back(new GBSDLWindow(new GBDebuggerView(world), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenAbout() {
	windows.push_back(new GBSDLWindow(new GBAboutBox(), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenSideDebugger() {
	windows.push_back(new GBSDLWindow(new GBSideDebuggerView(world), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenRoster() {
	windows.push_back(new GBSDLWindow(new GBRosterView(world), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenScores() {
	windows.push_back(new GBSDLWindow(new GBScoresView(world), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenTypeWindow() {
	windows.push_back(new GBSDLWindow(new GBRobotTypeView(world), 0, 0, true, this, false, &fontmanager));
}
void GBSDLApplication::OpenTournament() {
	windows.push_back(new GBSDLWindow(new GBTournamentView(world), 0, 0, true, this, false, &fontmanager));
}

#endif
