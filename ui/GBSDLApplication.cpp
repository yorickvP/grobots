// GBSDLApplication.cpp
// Grobots - GBSDLApplicationClass.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Grobots (non-copyright) GBSDLApplication 2010 Yorick
// Distributed under the GNU General Public License.

#include "GBPlatform.h"
#include "GBSDLApplication.h"
#include "GBErrors.h"

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
	: alive(true), clicks(0),
	clickx(0), clicky(0), world(), dragging(nil),
	portal(nil), mainView(nil), stepPeriod(-1), lastStep(0) {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) FatalError("Unable to init SDL");
	atexit(SDL_Quit);
	SDL_initFramerate(&stepManager);
	SetStepPeriod(kNormalSpeedLimit);
	
	portal = new GBPortal(world);
	mainView = new GBDoubleBufferedView(portal);
	SDL_WM_SetCaption("Grobots", nil);
	main_screen = SDL_SetVideoMode(mainView->PreferredWidth(), mainView->PreferredHeight(), 32, SDL_SWSURFACE | SDL_RESIZABLE);
	mainView->SetGraphics(new GBGraphics(main_screen));
	mainView->SetBounds(GBRect(0,0,mainView->PreferredWidth(), mainView->PreferredHeight()));
}
GBSDLApplication::~GBSDLApplication() {
	delete &mainView->Graphics();
	delete mainView; // this deletes the portal too
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
		} while ( world.running && (stepPeriod <= 0 || stepPeriod <= 10 && steps < kMaxFasterSteps)
			&& Milliseconds() <= lastStep + kMaxEventInterval );
	} catch ( GBError & err ) {
		NonfatalError("Error simulating: " + err.ToString());
	} catch ( GBAbort & ) {
		world.running = false;
	}
}
void GBSDLApplication::Redraw() {
	if (mainView->NeedsRedraw(world.running)) {
		mainView->DoDraw();
		SDL_Flip(main_screen);
	}
}
void GBSDLApplication::HandleEvent(SDL_Event* evt) {
	try {
		switch ( evt->type ) {
			case SDL_MOUSEBUTTONDOWN:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					ExpireClicks(evt->button.x, evt->button.y);
					clickTime = Milliseconds();
					clickx = evt->button.x; clicky = evt->button.y;
					dragging = mainView;
					mainView->DoClick(evt->button.x, evt->button.y, clicks);
					++clicks;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					if ( dragging ) {
						ExpireClicks(evt->button.x, evt->button.y);
						dragging->DoUnclick(evt->button.x, evt->button.y, clicks);
						dragging = nil;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if ( dragging ) {
					dragging->DoDrag(evt->motion.x, evt->motion.y);
				}
				break;
			case SDL_VIDEORESIZE:
				mainView->SetSize(evt->resize.w, evt->resize.h);
				delete &mainView->Graphics();
				main_screen = SDL_SetVideoMode(evt->resize.w, evt->resize.h, 32, SDL_SWSURFACE | SDL_RESIZABLE);
				mainView->SetGraphics(new GBGraphics(main_screen));
			default:
				break;
		}
	} catch ( GBError & err ) {
		NonfatalError("Error processing event: " + err.ToString());
	}
}
void GBSDLApplication::ExpireClicks(int x, int y) {
	if ( clicks && (Milliseconds() > clickTime +
			200
			|| abs(x - clickx) > kClickRange
			|| abs(y - clicky) > kClickRange) )
		clicks = 0;
}
