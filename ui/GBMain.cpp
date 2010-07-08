// GBMain.cpp
// main for Grobots with GBApplication.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifdef WITH_SDL
#include "GBSDLApplication.h"
#else
#include "GBApplication.h"
#endif
#include "GBErrors.h"
#include <exception>
#ifdef WITH_SDL
int main() {
	try {
		GBSDLApplication app;
		app.Run();
	} catch ( GBError & err ) {
		FatalError("Uncaught GBError: " + err.ToString());
	} catch ( GBRestart & r ) {
		FatalError("Uncaught GBRestart: " + r.ToString());
	} catch ( std::exception & e ) {
		FatalError("Uncaught std::exception: " + string(e.what()));
	}
#if !WINDOWS //this interferes with debugging on Windows
	catch ( ... ) {
		FatalError("Uncaught mystery exception.");
	}
#endif
	return EXIT_SUCCESS;
}

#else

#if WINDOWS
	int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int showCmd) {
#else
	int main() {
#endif
	try {
#if WINDOWS
		GBApplication app(hInstance, showCmd);
#else
		GBApplication app;
#endif		
		app.Run();
	} catch ( GBError & err ) {
		FatalError("Uncaught GBError: " + err.ToString());
	} catch ( GBRestart & r ) {
		FatalError("Uncaught GBRestart: " + r.ToString());
	} catch ( std::exception & e ) {
		FatalError("Uncaught std::exception: " + string(e.what()));
	}
#if !WINDOWS //this interferes with debugging on Windows
	catch ( ... ) {
		FatalError("Uncaught mystery exception.");
	}
#endif
	return EXIT_SUCCESS;
}
#endif

// that was short.
