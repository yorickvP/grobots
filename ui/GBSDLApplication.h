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
#include "SDL2_framerate.h"
#include <list>
#include <memory>

template <class T> using Ref = std::shared_ptr<T>;
union SDL_Event;
class GBSDLWindow;

class GBSDLApplication {
	bool alive;
	FPSmanager stepManager;
	int clicks; // how many mousedowns recently
	GBMilliseconds clickTime;
	short clickx, clicky;
	void HandleEvent(SDL_Event* evt);
	void ExpireClicks(int x, int y);
	void Process(); // do periodic processing
	void Redraw();
  Ref<GBSDLWindow> FindWndAtPos(short x, short y);
  Ref<GBSDLWindow> FindWndFromID(Uint32 id);
	
	GBMilliseconds stepPeriod;
	GBMilliseconds lastStep;
	
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
public:
	GBSDLApplication();
  GBSDLApplication(const GBSDLApplication&) = delete;
	~GBSDLApplication();
	void Run();
	void Quit();
	void SetStepPeriod(int period);

  void HandleMenuSelection(int item);
	
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
