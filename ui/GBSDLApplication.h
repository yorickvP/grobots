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
#include "GBSDLWindow.h"
#include "GBFontManager.h"
#include "GBMultiView.h"
#include "SDL.h"
#include "SDL2_framerate.h"
#include <list>

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
	GBSDLWindow* FindWndAtPos(short x, short y);
    GBSDLWindow* FindWndFromID(Uint32 id);
	
	GBMilliseconds stepPeriod;
	GBMilliseconds lastStep;
	
	GBFontManager fontmanager;
	
	GBSDLWindow * dragging;

	GBWorld world;
	
	GBPortal * portal;
	GBSDLWindow * mainWnd;
	GBSDLWindow * menuWnd;
	GBSDLWindow * focus;
	std::list<GBSDLWindow*> windows;
	
	void CloseWindow(GBSDLWindow* window);
  GBMultiView * mainView;
public:
	GBSDLApplication();
	~GBSDLApplication();
	void Run();
	void Quit();
	void SetStepPeriod(int period);

	
	void OpenMinimap();
	void OpenDebugger();
	void OpenAbout();
	void OpenSideDebugger();
	void OpenRoster();
	void OpenScores();
	void OpenTypeWindow();
	void OpenTournament();

  static void mainloop(void* arg);
};


#endif

#endif
