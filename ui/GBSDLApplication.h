// GBSDLApplication.h
// Grobots - GBSDLApplicationClass.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Grobots (non-copyright) GBSDLApplication 2010 Yorick
// Distributed under the GNU General Public License.
#ifdef WITH_SDL
#ifndef GBSDLApplication_h
#define GBSDLApplication_h
#include "GBTypes.h"
#include "GBWorld.h"
#include "GBView.h"
#include "GBSide.h"
#include "GBPortal.h"
#include "GBFontManager.h"
#include "GBMultiView.h"
#include <SDL3/SDL.h>
#include <list>
#include <memory>

template <class T> using Ref = std::shared_ptr<T>;
union SDL_Event;
class GBSDLWindow;
class GBMenuView;
class GBDebuggerView;
class GBMiniMapView;
class GBSideDebuggerView;

class GBSDLApplication {
	bool alive;
	int clicks; // how many mousedowns recently
	GBMilliseconds clickTime;
	short clickx, clicky;
	void HandleEvent(SDL_Event* evt);
	void ExpireClicks(int x, int y);
	void Process(); // do periodic processing
	bool Redraw();
  Ref<GBSDLWindow> FindWndAtPos(short x, short y);
  Ref<GBSDLWindow> FindWndFromID(Uint32 id);

	double tickRate;
	double dt;
	double simTime;
	double accumulator;
	uint64_t lastFrameTime;
	bool unlimitedSpeed;

	GBFontManager fontmanager;

  std::weak_ptr<GBSDLWindow> dragging;

	GBWorld world;

	Ref<GBPortal> portal;
  Ref<GBSDLWindow> mainWnd;
	Ref<GBSDLWindow> menuWnd;
  std::weak_ptr<GBSDLWindow> focus;
	std::list<Ref<GBSDLWindow>> windows;

	void CloseWindow(Ref<GBSDLWindow> window);
  Ref<GBMultiView> mainView;

  // Stored view references for menu state and actions
  Ref<GBMenuView> menuView;
  Ref<GBDebuggerView> debugger;
  Ref<GBMiniMapView> minimap;
  Ref<GBSideDebuggerView> sideDebugger;

  void DoReloadSide();
  void UpdateCursor();
public:
	GBSDLApplication();
  GBSDLApplication(const GBSDLApplication&) = delete;
	~GBSDLApplication();
	void Run();
	void Quit();
	void SetTickRate(double rate);
	void SetUnlimitedSpeed();

  void HandleMenuSelection(int item);
  void AdjustMenus();

  void OpenView(Ref<GBView> view, short x, short y);
	void OpenMinimap();
	void OpenDebugger();
	void OpenAbout();
	void OpenSideDebugger();
	void OpenRoster();
	void OpenScores();
	void OpenTypeWindow();
	void OpenTournament();
  void CloseView(const GBView& v);
  void DoLoadSide();

  static void mainloop(void* arg);
};


#endif

#endif
