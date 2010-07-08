// GBSDLApplication.h
// Grobots - GBSDLApplicationClass.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Grobots (non-copyright) GBSDLApplication 2010 Yorick
// Distributed under the GNU General Public License.

#ifndef GBSDLApplication_h
#define GBSDLApplication_h
#include "GBTypes.h"
#include "GBWorld.h"
#include "GBView.h"
#include "GBSide.h"
#include "GBPortal.h"
#include <SDL/SDL.h>
#include <SDL_framerate.h>

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
	
	GBMilliseconds stepPeriod;
	GBMilliseconds lastStep;
	
	SDL_Surface* main_screen;
	
	GBView * dragging;

	GBWorld world;
	GBPortal * portal;
	GBView * mainView;
public:
	GBSDLApplication();
	~GBSDLApplication();
	void Run();
	void Quit();
	void SetStepPeriod(int period);
};


#endif
