// GBMilliseconds.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBPlatform.h"
#include "GBMilliseconds.h"

#ifdef WITH_SDL
	#include "SDL.h"
#elif UNIX || WINDOWS || __EMSCRIPTEN__
  #include <time.h>
#endif

#ifdef WITH_SDL
GBMilliseconds Milliseconds() {
	return (GBMilliseconds)(SDL_GetTicks());
}
#elif MAC
GBMilliseconds Milliseconds() {
	return (GBMilliseconds)(TickCount()) * 1000 / 60;
}
#elif UNIX || WINDOWS || __EMSCRIPTEN__
GBMilliseconds Milliseconds() { return clock(); }
//FIXME this is process time, not real time
#else
	#warning "Need Milliseconds"
#endif

