// GBSDLApplication.cpp
// Grobots - GBSDLApplicationClass.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Grobots (non-copyright) GBSDLApplication 2010 Yorick
// Distributed under the GNU General Public License.
#ifdef WITH_SDL
#include "GBPlatform.h"
#include "GBSDLApplication.h"
#include "GBSDLWindow.h"
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
// todo: duplicated from GBApplication.h
//Menu item IDs: these are 100*menuid + itemposition
enum {
	kFileMenu = 129,
		miLoadSide = 12901, miDuplicateSide,
		miReloadSide = 12903,
		miRemoveSide = 12905, miRemoveAllSides,
		miClose = 12908,
		miQuit = 12910,
	kWindowMenu = 130,
		miAbout = 13001,
		miRosterView, miMainView, miMinimapView,
		miScoresView, miTypesView,
		miDebuggerView,
		miTournamentView, miSideDebuggerView,
	kViewMenu = 131,
		miSound = 13101,
		miZoomIn = 13103, miZoomOut, miZoomStandard,
		miShowSensors = 13107, miShowDecorations, miShowMeters,
		miMinimapTrails,
		miReportErrors = 13112, miReportPrints,
		miRefollow = 13115, miFollowRandom, miRandomNear, miAutofollow,
		miNextPageMemory = 13120, miPreviousPageMemory, miFirstPageMemory,
	kSimulationMenu = 132,
		miRun = 13201, miSingleFrame, miStep, miPause, miStartStopBrain,
		miSlowerSpeed = 13207, miSlowSpeed, miNormalSpeed, miFastSpeed, miFasterSpeed, miUnlimitedSpeed,
		miNewRound = 13214, miRestart,
		miSeed, miReseed,
		miRules = 13219,
	miTournament = 13220, miSaveScoresHtml, miSaveScoresWiki, miResetScores,
	kToolsMenu = 133,
		miScroll = 13301,
		miAddManna = 13303, miAddRobot, miAddSeed,
		miMove = 13307, miPull, miSmite, miBlasts, miErase, miEraseBig
};

const short kClickRange = 5;
const double kSlowerTickRate = 2.0;
const double kSlowTickRate = 10.0;
const double kNormalTickRate = 30.0;
const double kFastTickRate = 60.0;
const double kFasterTickRate = 300.0;

const GBMilliseconds kMaxEventInterval = 50;

GBSDLApplication::GBSDLApplication()
	: alive(true), clicks(0), clickx(0), clicky(0),
    tickRate(kNormalTickRate), dt(1.0 / kNormalTickRate), simTime(0.0),
    accumulator(0.0), lastFrameTime(0), unlimitedSpeed(false),
    fontmanager(), dragging(), world(), focus(), windows() {
  // TODO: find out which parts to init
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) FatalError("Unable to init SDL");
	atexit(SDL_Quit);

	portal = std::make_shared<GBPortal>(world);
	mainView = std::make_shared<GBMultiView>(portal);

	mainWnd = std::make_shared<GBSDLWindow>(mainView, true, true, fontmanager);
  menuView = std::make_shared<GBMenuView>(*this, fontmanager);
  mainView->Add(menuView, -1, -1);
	focus = mainWnd;
	windows.push_back(mainWnd);
  OpenMinimap();
  OpenRoster();
}
GBSDLApplication::~GBSDLApplication() {}

void GBSDLApplication::mainloop(void* arg) {
  GBSDLApplication *app = static_cast<GBSDLApplication*>(arg);

  uint64_t now = SDL_GetPerformanceCounter();
  double frameTime = (double)(now - app->lastFrameTime) / SDL_GetPerformanceFrequency();
  app->lastFrameTime = now;

  // clamp to avoid spiral of death
  if (frameTime > 0.25) frameTime = 0.25;
  app->accumulator += frameTime;

  // poll events
  SDL_Event event;
  while (app->alive && SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_EVENT_QUIT:
      app->Quit();
      return;
    default:
      app->HandleEvent(&event);
    }
  }

  // simulation ticks
  if (app->unlimitedSpeed) {
    // run as many frames as possible within a time budget
    GBMilliseconds start = Milliseconds();
    while (app->world.running && Milliseconds() <= start + kMaxEventInterval) {
      app->Process();
    }
    app->accumulator = 0.0;
  } else {
    while (app->accumulator >= app->dt) {
      app->Process();
      app->simTime += app->dt;
      app->accumulator -= app->dt;
    }
  }

  // render
#ifdef __EMSCRIPTEN__
  // emscripten main loop is already paced by requestAnimationFrame,
  // so always redraw — no need to idle-wait (which busy-spins on web)
  app->Redraw();
#else
  // vsync blocks in present when something is drawn
  if (!app->Redraw()) {
    // nothing was drawn — wait for events or next tick to avoid busy spin
    int waitMs = 1;
    if (!app->unlimitedSpeed && app->world.running) {
      double timeToNextTick = app->dt - app->accumulator;
      if (timeToNextTick > 0)
        waitMs = (int)(timeToNextTick * 1000);
      if (waitMs < 1) waitMs = 1;
    } else if (!app->world.running) {
      waitMs = 16; // idle when paused
    }
    SDL_WaitEventTimeout(nullptr, waitMs);
  }
#endif
}

void GBSDLApplication::Run() {
  lastFrameTime = SDL_GetPerformanceCounter();
  #if __EMSCRIPTEN__
  emscripten_set_main_loop_arg(GBSDLApplication::mainloop, this, -1, 1);
  #else
  do {
    GBSDLApplication::mainloop(this);
  } while (alive);
  #endif
}

void GBSDLApplication::SetTickRate(double rate) {
  tickRate = rate;
  dt = 1.0 / rate;
  unlimitedSpeed = false;
}

void GBSDLApplication::SetUnlimitedSpeed() {
  unlimitedSpeed = true;
}

void GBSDLApplication::Quit() {
	if (world.tournament) {
		SDL_MessageBoxButtonData buttons[] = {
			{SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "Cancel"},
			{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Quit"},
		};
		SDL_MessageBoxData data = {};
		data.flags = SDL_MESSAGEBOX_WARNING;
		data.title = "Quit during tournament?";
		data.message = "A tournament is in progress. Are you sure you want to quit?";
		data.numbuttons = 2;
		data.buttons = buttons;
		int buttonid = 0;
		SDL_ShowMessageBox(&data, &buttonid);
		if (buttonid != 1) return;
	}
	alive = false;
}

void GBSDLApplication::Process() {
	if ( !world.running ) return;
	try {
		world.AdvanceFrame();
	} catch ( GBError & err ) {
		NonfatalError("Error simulating: " + err.ToString());
	} catch ( GBAbort & ) {
		world.running = false;
	}
}
bool GBSDLApplication::Redraw() {
	bool any = false;
	bool isDragging = !dragging.expired();
	for (auto it = windows.begin(); it != windows.end(); ++it) {
		if (!(*it)->Visible()) continue;
		if ((*it)->DrawChanges(world.running || isDragging)) any = true;
	}
	return any;
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

void GBSDLApplication::UpdateCursor() {
	static SDL_Cursor* cursors[4] = {nullptr};
	if (!cursors[0]) {
		cursors[cuArrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
		cursors[cuHand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
		cursors[cuCross] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
		cursors[cuWait] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	}
	GBCursor c = mainView->Cursor();
	SDL_SetCursor(cursors[c]);
}

void GBSDLApplication::HandleEvent(SDL_Event* evt) {
	try {
		switch ( evt->type ) {
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
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
    case SDL_EVENT_MOUSE_WHEEL: {
      Ref<GBSDLWindow> wnd = FindWndFromID(evt->wheel.windowID);
      // todo: focus
      if (!wnd) break;
      if (evt->wheel.y > 0) {
        wnd->DoZoom((short)evt->wheel.mouse_x, (short)evt->wheel.mouse_y, 1);
      } else if (evt->wheel.y < 0) {
        wnd->DoZoom((short)evt->wheel.mouse_x, (short)evt->wheel.mouse_y, -1);
      }
      // todo: smooth zooming using preciseY
    } break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				if (evt->button.button == SDL_BUTTON_LEFT) {
					if ( auto d = dragging.lock() ) {
						ExpireClicks(evt->button.x, evt->button.y);
						d->AcceptUnclick(evt->button.x, evt->button.y, clicks);
						dragging = {};
					}
				}
				break;
			case SDL_EVENT_MOUSE_MOTION:
				if (evt->motion.state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) {
					if (auto d = dragging.lock() ) {
						d->AcceptDrag(evt->motion.x, evt->motion.y);
					}
				}
				UpdateCursor();
				break;
			case SDL_EVENT_KEY_DOWN: {
				// Check for global keyboard shortcuts first
				if (menuView) {
					int menuId = menuView->FindShortcut(evt->key.key, evt->key.mod);
					if (menuId != 0) {
						HandleMenuSelection(menuId);
						break;
					}
				}
				// Fall through to normal keystroke handling
				if (auto f = focus.lock()) {
					f->AcceptKeystroke(evt->key.key);
				}
			}	break;
    case SDL_EVENT_WINDOW_SHOWN:
    case SDL_EVENT_WINDOW_EXPOSED: {
        Ref<GBSDLWindow> wnd = FindWndFromID(evt->window.windowID);
        if (wnd) wnd->Update(false);
        break; }
    case SDL_EVENT_WINDOW_RESIZED: {
        Ref<GBSDLWindow> wnd = FindWndFromID(evt->window.windowID);
        if (wnd) wnd->SetSize(evt->window.data1, evt->window.data2);
        break; }
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
        Ref<GBSDLWindow> wnd = FindWndFromID(evt->window.windowID);
        if (wnd) this->CloseWindow(wnd);
        break; }
    case SDL_EVENT_WINDOW_FOCUS_GAINED: {
        Ref<GBSDLWindow> wnd = FindWndFromID(evt->window.windowID);
        if (wnd) wnd->SetFocus(true);
        break; }
    case SDL_EVENT_WINDOW_FOCUS_LOST: {
        Ref<GBSDLWindow> wnd = FindWndFromID(evt->window.windowID);
        if (wnd) wnd->SetFocus(false);
        break; }
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
void GBSDLApplication::OpenView(Ref<GBView> view, short x, short y) {
  mainView->Add(view, x, y);
}

void GBSDLApplication::CloseView(const GBView& v) {
  // todo: multi window
  mainView->CloseView(v);
}
void GBSDLApplication::OpenMinimap() {
  if (!minimap) minimap = std::make_shared<GBMiniMapView>(world, *portal);
  mainView->Add(minimap, 7, mainView->Bounds().bottom - 200);
}
void GBSDLApplication::OpenDebugger() {
  if (!debugger) debugger = std::make_shared<GBDebuggerView>(world);
	mainView->Add(debugger, 616, 43);
}
void GBSDLApplication::OpenAbout() {
  // TODO: center
  short x = mainView->Bounds().CenterX()-131;
  short y = mainView->Bounds().CenterY() - 118;
	mainView->Add(std::make_shared<GBAboutBox>(), x, y);
}
void GBSDLApplication::OpenSideDebugger() {
  if (!sideDebugger) sideDebugger = std::make_shared<GBSideDebuggerView>(world);
	mainView->Add(sideDebugger, 200, 400);
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

void GBSDLApplication::DoReloadSide() {
	GBSide * oldSide = world.SelectedSide();
	if ( ! oldSide ) return;
	GBSide * newSide = GBSideReader::Load(oldSide->filename);
	if ( newSide ) {
		if ( oldSide->Scores().Seeded() ) {
			world.Reset();
			world.running = false;
		}
		world.ReplaceSide(oldSide, newSide);
	}
}

static void LoadSideCallback(void *userdata, const char * const *filelist, int /*filter*/) {
  GBWorld *world = static_cast<GBWorld *>(userdata);
  if (!filelist) return;
  for (const char * const *f = filelist; *f; ++f) {
    GBSide *side = GBSideReader::Load(*f);
    if (side) world->AddSide(side);
  }
}

void GBSDLApplication::DoLoadSide() {
#ifdef __EMSCRIPTEN__
  world.running = false;
  EM_ASM({
  const input = document.createElement('input');
  input.type = 'file';
  input.multiple = true;
  input.accept = '.gb';
  input.onchange = _ => {
    const files = Array.from(input.files);
    files.forEach(file => {
        file.text().then(txt =>
                         Module.ccall('addSide', 'null', ['number', 'string', 'string'], [$0, file.name, txt]));
    })
  };
  input.click();
    }, &world);
#else
  static const SDL_DialogFileFilter filters[] = {
    { "Grobots sides", "gb" },
    { "All files", "*" },
  };
  SDL_ShowOpenFileDialog(LoadSideCallback, &world, nullptr, filters, 2, nullptr, true);
#endif
}

void GBSDLApplication::AdjustMenus() {
	if (!menuView) return;
	// File menu
	menuView->EnableItem(miDuplicateSide, world.SelectedSide() != nullptr);
	menuView->EnableItem(miReloadSide, world.SelectedSide() != nullptr);
	menuView->EnableItem(miRemoveSide, world.SelectedSide() != nullptr);
	menuView->EnableItem(miRemoveAllSides, world.CountSides() != 0);
	// View menu
	menuView->EnableItem(miSound, false); // no sound support yet
	menuView->CheckItem(miShowSensors, portal->showSensors);
	menuView->CheckItem(miShowDecorations, portal->showDecorations);
	menuView->CheckItem(miShowMeters, portal->showDetails);
	if (minimap) menuView->CheckItem(miMinimapTrails, minimap->showTrails);
	menuView->CheckItem(miReportErrors, world.reportErrors);
	menuView->CheckItem(miReportPrints, world.reportPrints);
	menuView->CheckItem(miAutofollow, portal->autofollow);
	menuView->EnableItem(miNextPageMemory, sideDebugger && sideDebugger->pane < 9);
	menuView->EnableItem(miPreviousPageMemory, sideDebugger && sideDebugger->pane > 0);
	menuView->EnableItem(miFirstPageMemory, sideDebugger && sideDebugger->pane != 0);
	// Simulation menu
	menuView->EnableItem(miRun, !world.running);
	menuView->EnableItem(miSingleFrame, !world.running);
	menuView->EnableItem(miStep, !world.running && debugger && debugger->Active());
	menuView->EnableItem(miPause, world.running);
	menuView->EnableItem(miStartStopBrain, debugger && debugger->Active());
	menuView->CheckItem(miSlowerSpeed, !unlimitedSpeed && tickRate == kSlowerTickRate);
	menuView->CheckItem(miSlowSpeed, !unlimitedSpeed && tickRate == kSlowTickRate);
	menuView->CheckItem(miNormalSpeed, !unlimitedSpeed && tickRate == kNormalTickRate);
	menuView->CheckItem(miFastSpeed, !unlimitedSpeed && tickRate == kFastTickRate);
	menuView->CheckItem(miFasterSpeed, !unlimitedSpeed && tickRate == kFasterTickRate);
	menuView->CheckItem(miUnlimitedSpeed, unlimitedSpeed);
	menuView->CheckItem(miTournament, world.tournament);
	// Tools menu
	GBSide * side = world.SelectedSide();
	menuView->EnableItem(miAddSeed, side != nullptr);
	menuView->EnableItem(miAddRobot, side != nullptr);
	if ( !side && (portal->tool == ptAddSeed || portal->tool == ptAddRobot) )
		portal->tool = ptScroll;
	menuView->CheckItem(miScroll, portal->tool == ptScroll);
	menuView->CheckItem(miAddManna, portal->tool == ptAddManna);
	menuView->CheckItem(miAddRobot, portal->tool == ptAddRobot);
	menuView->CheckItem(miAddSeed, portal->tool == ptAddSeed);
	menuView->CheckItem(miMove, portal->tool == ptMove);
	menuView->CheckItem(miPull, portal->tool == ptPull);
	menuView->CheckItem(miSmite, portal->tool == ptSmite);
	menuView->CheckItem(miBlasts, portal->tool == ptBlasts);
	menuView->CheckItem(miErase, portal->tool == ptErase);
	menuView->CheckItem(miEraseBig, portal->tool == ptEraseBig);
	// Rules - always disabled (no dialog yet)
	menuView->EnableItem(miRules, false);
}

void GBSDLApplication::HandleMenuSelection(int item) {
	try {
		switch ( item ) {
		//Apple or Help menu
			case miAbout: OpenAbout(); break;
		//File menu
      case miLoadSide: DoLoadSide(); break;
			case miDuplicateSide:
				if ( world.SelectedSide() )
					world.AddSide(world.SelectedSide()->Copy());
				break;
			case miReloadSide: DoReloadSide(); break;
			case miRemoveSide:
				if ( world.SelectedSide() ) {
					if ( world.SelectedSide()->Scores().Seeded() ) {
						world.Reset();
						world.running = false;
					}
					world.RemoveSide(world.SelectedSide());
				} break;
			case miRemoveAllSides:
				world.Reset();
				world.RemoveAllSides();
				world.running = false;
				break;
			case miQuit:
				Quit();
				break;
		//Window menu
    case miRosterView: OpenRoster(); break;
    case miMinimapView: OpenMinimap(); break;
    case miScoresView: OpenScores(); break;
    case miTypesView: OpenTypeWindow(); break;
    case miTournamentView: OpenTournament(); break;
    case miDebuggerView: OpenDebugger(); break;
    case miSideDebuggerView: OpenSideDebugger(); break;
		//View menu
			case miZoomIn: portal->Zoom(1); break;
			case miZoomOut: portal->Zoom(-1); break;
			case miZoomStandard: portal->ResetZoom(); break;
			case miShowSensors: portal->showSensors = ! portal->showSensors; break;
			case miShowDecorations: portal->showDecorations = ! portal->showDecorations; break;
			case miShowMeters: portal->showDetails = ! portal->showDetails; break;
			case miMinimapTrails:
				if (minimap) minimap->showTrails = ! minimap->showTrails;
				break;
			case miReportErrors: world.reportErrors = ! world.reportErrors; break;
			case miReportPrints: world.reportPrints = ! world.reportPrints; break;
			case miRefollow: portal->Refollow(); break;
			case miFollowRandom: portal->FollowRandom(); break;
			case miRandomNear: portal->FollowRandomNear(); break;
			case miAutofollow: portal->autofollow = ! portal->autofollow; break;
			case miNextPageMemory:
				if (sideDebugger && sideDebugger->pane < 9) ++sideDebugger->pane;
				break;
			case miPreviousPageMemory:
				if (sideDebugger && sideDebugger->pane > 0) --sideDebugger->pane;
				break;
			case miFirstPageMemory:
				if (sideDebugger) sideDebugger->pane = 0;
				break;
		//Simulation menu:
			case miRun:
				world.running = true;
				break;
			case miSingleFrame:
				world.AdvanceFrame();
				world.running = false;
				break;
			case miStep:
				if ( debugger && debugger->Active() && debugger->Step() )
					world.AdvanceFrame();
				world.running = false;
				break;
			case miPause: world.running = false; break;
			case miStartStopBrain:
				if (debugger) debugger->StartStopBrain();
				break;
			case miSlowerSpeed: SetTickRate(kSlowerTickRate); break;
			case miSlowSpeed: SetTickRate(kSlowTickRate); break;
			case miNormalSpeed: SetTickRate(kNormalTickRate); break;
			case miFastSpeed: SetTickRate(kFastTickRate); break;
			case miFasterSpeed: SetTickRate(kFasterTickRate); break;
			case miUnlimitedSpeed: SetUnlimitedSpeed(); break;
			case miNewRound:
				world.Reset();
				world.AddSeeds();
				world.running = true;
				break;
			case miRestart:
				world.Reset();
				world.running = false;
				break;
			case miSeed: world.AddSeeds(); break;
			case miReseed: world.ReseedDeadSides(); break;
			case miTournament:
				if ( world.tournament ) world.tournament = false;
				else world.tournament = true;
				break;
			case miSaveScoresHtml: world.DumpTournamentScores(true); break;
			case miSaveScoresWiki: world.DumpTournamentScores(false); break;
			case miResetScores: world.ResetTournamentScores(); break;
		//Tools menu
			case miScroll: portal->tool = ptScroll; break;
			case miAddManna: portal->tool = ptAddManna; break;
			case miAddRobot: portal->tool = ptAddRobot; break;
			case miAddSeed: portal->tool = ptAddSeed; break;
			case miMove: portal->tool = ptMove; break;
			case miPull: portal->tool = ptPull; break;
			case miSmite: portal->tool = ptSmite; break;
			case miBlasts: portal->tool = ptBlasts; break;
			case miErase: portal->tool = ptErase; break;
			case miEraseBig: portal->tool = ptEraseBig; break;
		//other
			default:
				break;
		}
	} catch ( GBAbort & ) {}
}

#endif
