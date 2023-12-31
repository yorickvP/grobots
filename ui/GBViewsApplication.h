#ifndef WITH_SDL
// GBViewsApplication.h
// Abstract top-level interface class.
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef ViewsApplication_h
#define ViewsApplication_h

#include "GBView.h"
#if MAC
class FSSpec;
#endif

class GBWindow;

enum GBMenuKeyModifier { modDefault = 0, modNone, modShift, };

struct GBMenuItem {
	int id;
	const char * name;
	char key;
	GBMenuKeyModifier modifier;
};

class GBViewsApplication {
protected:
	bool alive;
	int clicks; // how many mousedowns recently
	GBMilliseconds clickTime;
	short clickx, clicky;
	GBWindow * dragging;
	GBMilliseconds stepPeriod;
	GBMilliseconds lastStep;
	GBWindow * mainWindow; //special on Windows but not on Mac
#if WINDOWS
	HINSTANCE hInstance;
	int showCmd;
	ACCEL accelKeys[1000];
	int numAccelKeys;
	HACCEL hAccelTable;
	HMENU mainMenu;
#endif
private:
#if MAC
// initialization
	void SetupAppleEvents();
// event handling
	void HandleEvent(EventRecord * evt);
	void HandleMouseDown(EventRecord * evt);
	void HandleMouseUpOrDrag(EventRecord * evt);
	void HandleKeyDown(EventRecord * evt);
	void HandleUpdate(EventRecord * evt);
	void AdjustCursor(Point where);
#endif
	void ExpireClicks(int x, int y);
public:
#if WINDOWS
	GBViewsApplication(HINSTANCE hInstance, int showCmd);
#else
	GBViewsApplication();
#endif
	virtual ~GBViewsApplication();
// operation
	void Run();
// useful
#if MAC
	static bool DoNumberDialog(ConstStr255Param prompt, long & value,
		long min = -1000000000, long max = 1000000000);
	static void SetCursor(GBCursor curs);
#elif WINDOWS
	GBWindow * MainWindow();
	static LRESULT CALLBACK WindowProc(HWND hWin, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
	void SetStepPeriod(int period);
//UI setup
	static GBRect GetScreenSize();
	GBWindow * MakeWindow(GBView * view, int x, int y, bool visible = true);
//menus
	void AddMenu(const GBMenuItem * items);
	void CheckOne(int item, bool checked);
	void EnableOne(int item, bool checked);
protected: // to override
	virtual void AdjustMenus();
	virtual void HandleMenuSelection(int item);
	virtual void Process(); // do periodic processing
	virtual void Redraw();
public: // more to override
#if MAC
	virtual long SleepTime() = 0;
	virtual void OpenApp();
	virtual void OpenFile(FSSpec & file);
	virtual void PrintFile(FSSpec & file);
#endif
	virtual void Quit();
};

#endif
#endif
