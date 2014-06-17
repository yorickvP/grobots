// GBViewsApplication.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBPlatform.h"
#if WINDOWS
#include <string.h>
#endif
#include "GBWindow.h"
#include "GBErrors.h"
#include "GBViewsApplication.h"
#include "GBStringUtilities.h"

const short kNumberDialogID = 128;

const short kClickRange = 5;

#if MAC
static pascal OSErr HandleAEOpenApplication(const AppleEvent * evt, AppleEvent * reply, long refCon);
static pascal OSErr HandleAEOpenDocuments(const AppleEvent * evt, AppleEvent * reply, long refCon);
static pascal OSErr HandleAEPrintDocuments(const AppleEvent * evt, AppleEvent * reply, long refCon);
static pascal OSErr HandleAEQuitApplication(const AppleEvent * evt, AppleEvent * reply, long refCon);


static pascal OSErr HandleAEOpenApplication(const AppleEvent * /*evt*/, AppleEvent * /*reply*/, long refCon) {
	((GBViewsApplication *)refCon)->OpenApp();
	return noErr;
}

static pascal OSErr HandleAEOpenDocuments(const AppleEvent * evt, AppleEvent * /*reply*/, long refCon) {
	OSErr err;
	AEDescList list;
	long items, dummySize;
	FSSpec spec;
	AEKeyword dummyKey;
	DescType dummyType;
	
	err = AEGetParamDesc(evt, keyDirectObject, typeAEList, &list);
	if ( err ) return err;		
	err = AECountItems(&list, &items);
	if ( err ) {
		AEDisposeDesc(&list);
		return err;
	}
	for ( long i = 1; i <= items; i ++ ) {
		err = AEGetNthPtr(&list, i, typeFSS, &dummyKey, &dummyType, (Ptr)&spec, sizeof(FSSpec), &dummySize);
		if ( err ) {
			AEDisposeDesc(&list);
			return err;
		}
		((GBViewsApplication *)refCon)->OpenFile(spec);
	}
	return noErr;
}

static pascal OSErr HandleAEPrintDocuments(const AppleEvent * /*evt*/, AppleEvent * /*reply*/, long refCon) {
	((GBViewsApplication *)refCon)->OpenApp();
	return noErr;
}

static pascal OSErr HandleAEQuitApplication(const AppleEvent * /*evt*/, AppleEvent * /*reply*/, long refCon) {
	((GBViewsApplication *)refCon)->Quit();
	return noErr;
}


void GBViewsApplication::SetupAppleEvents() {
	if ( AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerUPP(&HandleAEOpenApplication), (long)this, false) )
		NonfatalError("couldn't install AppleEvent handler");
	if ( AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerUPP(&HandleAEOpenDocuments), (long)this, false) )
		NonfatalError("couldn't install AppleEvent handler");
	if ( AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerUPP(&HandleAEPrintDocuments), (long)this, false) )
		NonfatalError("couldn't install AppleEvent handler");
	if ( AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(&HandleAEQuitApplication), (long)this, false) )
		NonfatalError("couldn't install AppleEvent handler");
}
#endif

#if MAC
void GBViewsApplication::HandleEvent(EventRecord * evt) {
	try {
		switch ( evt->what ) {
			case mouseDown:
				HandleMouseDown(evt);
				break;
			case mouseUp:
				if ( dragging ) {
					ExpireClicks(evt->where.h, evt->where.v);
					dragging->AcceptUnclick(evt->where, clicks);
					dragging = nil;
				}
				break;
			case keyDown:
			case autoKey:
				HandleKeyDown(evt);
				break;
			case updateEvt:
				HandleUpdate(evt);
				break;
			case kHighLevelEvent:
				//This is failing on OSX (from an unexxpected event?).
				if ( AEProcessAppleEvent(evt) )
					;//NonfatalError("couldn't process AppleEvent");
				break;
			case nullEvent:
				break;
			default:
				break;
		}
	} catch ( GBError & err ) {
		NonfatalError("Error processing event: " + err.ToString());
	}
}

void GBViewsApplication::HandleMouseDown(EventRecord * evt) {
	WindowPtr window;
	switch ( FindWindow(evt->where, &window) ) {
		case inMenuBar:
			AdjustMenus();
			{
				long result = MenuSelect(evt->where);
				HandleMenuSelection(100 * HiWord(result) + LoWord(result));
				HiliteMenu(0);
			}
			break;
		case inContent:
			ExpireClicks(evt->where.h, evt->where.v);
			clickTime = Milliseconds();
			clickx = evt->where.h; clicky = evt->where.v;
			{
				bool isFront = window == FrontWindow();
				if ( ! isFront )
					SelectWindow(window);
				if ( GBWindow::IsGBWindow(window) ) {
					GBWindow * mw = GBWindow::GetFromWindow(window);
					if ( isFront || mw->GetFrontClicks() ) {
						dragging = mw;
						mw->AcceptClick(evt->where, clicks);
					}
				}
			}
			++ clicks;
			break;
		case inDrag:
			if ( ! evt->modifiers & cmdKey )
				SelectWindow(window);
			DragWindow(window, evt->where, nil);
			break;
		case inGoAway:
			if ( TrackGoAway(window, evt->where) )
				if ( GBWindow::IsGBWindow(window) )
					GBWindow::GetFromWindow(window)->Hide();
			break;
		case inGrow:
			if ( GBWindow::IsGBWindow(window) )
				GBWindow::GetFromWindow(window)->Resize(evt->where);
			break;
		case inZoomIn:
			if ( TrackBox(window, evt->where, inZoomIn) )
				if ( GBWindow::IsGBWindow(window) )
					GBWindow::GetFromWindow(window)->ZoomIn();
		case inZoomOut:
			if ( TrackBox(window, evt->where, inZoomOut) )
				if ( GBWindow::IsGBWindow(window) )
					GBWindow::GetFromWindow(window)->ZoomOut();
			break;
		default:
			break;
	}
}
void GBViewsApplication::HandleKeyDown(EventRecord * evt) {
	//TODO if ( IsMenuKeyEvent(NULL, evt, kNilOptions, NULL, NULL) ) {
	if ( evt->modifiers & cmdKey ) {
		if ( evt->what == keyDown ) {
			AdjustMenus();
			long result = MenuKey(evt->message & charCodeMask);
			HandleMenuSelection(100 * HiWord(result) + LoWord(result));
			HiliteMenu(0);
		}
	} else {
		WindowPtr wind = FrontWindow();
		if ( GBWindow::IsGBWindow(wind) )
			GBWindow::GetFromWindow(wind)->AcceptKeystroke(evt->message & charCodeMask);
	}
}

void GBViewsApplication::HandleUpdate(EventRecord * evt) {
	WindowPtr window = (WindowPtr)(evt->message);
	if ( GBWindow::IsGBWindow(window) )
		GBWindow::GetFromWindow(window)->Update(false);
}

void GBViewsApplication::AdjustCursor(Point where) {
}
#endif

void GBViewsApplication::ExpireClicks(int x, int y) {
	if ( clicks && (Milliseconds() > clickTime +
#if MAC
			GetDblTime() * 1000 / 60
#else
			200
#endif
			|| abs(x - clickx) > kClickRange
			|| abs(y - clicky) > kClickRange) )
		clicks = 0;
}

#if WINDOWS
GBWindow * GBViewsApplication::MainWindow() {
	return mainWindow;
}

WNDCLASS wclass = {CS_OWNDC | CS_VREDRAW | CS_HREDRAW, GBViewsApplication::WindowProc,
	0, 0, 0, 0, 0, 0, 0, kWindowClassName};

HRESULT CALLBACK GBViewsApplication::WindowProc(HWND hWin, UINT msg,
												WPARAM wParam, LPARAM lParam) {
	GBWindow * self = reinterpret_cast<GBWindow *>(GetWindowLong(hWin, GWL_USERDATA));
	int x = LOWORD(lParam), y = HIWORD(lParam);
	switch (msg) {
		case WM_CLOSE:
			if (self->isMain)
				self->app->Quit();
			else
				self->Hide();
			break;
		//case WM_DESTROY:
		//	break;
		case WM_PAINT:
			self->Update(true);
			break;
		case WM_SIZE:
			self->SetSize(x, y);
			break;
		case WM_LBUTTONDOWN:
			self->app->ExpireClicks(x, y);
			self->app->clickTime = Milliseconds();
			self->app->clickx = x;
			self->app->clicky = y;
			self->AcceptClick(x, y, self->app->clicks);
			self->app->dragging = self;
			++self->app->clicks;
			break;
		case WM_MOUSEMOVE:
			//adjust cursor
			if (self->app->dragging == self)
				self->AcceptDrag(x, y);
			break;
		case WM_LBUTTONUP:
			self->app->ExpireClicks(x, y);
			self->AcceptUnclick(x, y, self->app->clicks);
			self->app->dragging = nil;
			break;
		case WM_KEYDOWN:
#ifndef MAPVK_VK_TO_CHAR
			#define MAPVK_VK_TO_CHAR 2 //should be in WinUser.h but it's not;
			// see http://www.codeguru.com/forum/archive/index.php/t-426785.html
#endif
			self->AcceptKeystroke(MapVirtualKey(wParam,MAPVK_VK_TO_CHAR));
			break;
		case WM_INITMENU:
			self->app->AdjustMenus();
			break;
		case WM_COMMAND:
			self->app->HandleMenuSelection(LOWORD(wParam));
			break;
		default:
			return DefWindowProc(hWin, msg, wParam, lParam);
	}
	return 0;
}
#endif

#if WINDOWS
GBViewsApplication::GBViewsApplication(HINSTANCE inst, int cmd)
	: hInstance(inst), showCmd(cmd),
#else
GBViewsApplication::GBViewsApplication() :
#endif
	alive(true),
	clicks(0), clickTime(0), clickx(0), clicky(0),
	dragging(nil),
	stepPeriod(-1),
	lastStep(0),
	mainWindow(nil)
{
#if MAC
	NavLoad();
#elif WINDOWS
	wclass.hInstance = inst;
	wclass.hIcon = LoadIcon(inst, MAKEINTRESOURCE(103));
	wclass.hCursor = LoadCursor(inst, IDC_ARROW);
	RegisterClass(&wclass);
#endif
}

GBViewsApplication::~GBViewsApplication() {
	delete mainWindow;
#if CARBON
	NavUnload();
#endif
}

GBRect GBViewsApplication::GetScreenSize() {
#if CARBON
	Rect bounds;
	if ( !GetAvailableWindowPositioningBounds(GetMainDevice(), &bounds) )
		return GBRect(bounds);
#elif WINDOWS
	RECT workArea;
	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0))
		return GBRect(workArea.left, workArea.top, workArea.right, workArea.bottom);
#endif
	return GBRect(2, 44, 1024, 768);
}

GBWindow * GBViewsApplication::MakeWindow(GBView * view, int x, int y, bool visible) {
#if WINDOWS
	GBWindow * w = new GBWindow(view, x, y, visible, hInstance, this);
	ShowWindow(w->win, visible ? showCmd : SW_HIDE);
	UpdateWindow(w->win);
	return w;
#else
	return new GBWindow(view, x, y, visible);
#endif
}

void GBViewsApplication::Run() {
#if MAC
	SetupAppleEvents();
	EventRecord event;
	do {
		Process();
		Redraw();
		while ( alive && WaitNextEvent(everyEvent, &event, SleepTime(), nil) ) {
			HandleEvent(&event);
			Redraw();
		}
		if ( dragging && Button() )
			dragging->AcceptDrag(event.where);
		AdjustCursor(event.where);
	} while ( alive );
#elif WINDOWS
	MSG msg;
 	while (alive && GetMessage(&msg, 0, 0, 0) > 0)
 		if (!TranslateAccelerator(MainWindow()->win, hAccelTable, &msg)) {
 			DispatchMessage(&msg);
 			if (msg.message == WM_TIMER)
 				Process();
 			Redraw();
 		}	
#endif
}

void GBViewsApplication::SetStepPeriod(int period) {
	stepPeriod = period;
#if WINDOWS
	if (! ::SetTimer(mainWindow->win, 1, period == 0 ? 10 : period < 0 ? 0 : period, 0)) //is this right?
		FatalError("Couldn't set timer.");
#endif
}

void GBViewsApplication::CheckOne(int item, bool checked) {
#if MAC
	MenuHandle thisMenu = GetMenuHandle(item / 100);
	CheckMenuItem(thisMenu, item % 100, checked);
#elif WINDOWS
	CheckMenuItem(GetMenu(mainWindow->win), item, checked ? MF_CHECKED : MF_UNCHECKED);
#endif
}

void GBViewsApplication::EnableOne(int item, bool enabled) {
#if MAC
	if (enabled) EnableMenuCommand(NULL, item);
	else DisableMenuCommand(NULL, item);
#elif WINDOWS
	EnableMenuItem(GetMenu(mainWindow->win), item, enabled ? MF_ENABLED : MF_GRAYED);
#endif
}

//First item is the id and title of the whole menu. 0 terminates. 
void GBViewsApplication::AddMenu(const GBMenuItem * items) {
#if CARBON
	Str255 str;
	ToPascalString(items[0].name, str);
	MenuRef menu = NewMenu(items[0].id, str);
	for ( int ix = 1; items[ix].id; ++ix ) {
		if ( items[ix].name ) {
			ToPascalString(items[ix].name, str);
			InsertMenuItemText(menu, str, ix);
		} else {
			ToPascalString("-", str);
			InsertMenuItemText(menu, str, ix);
		}
		if ( items[ix].id )
			SetMenuItemCommandID(menu, ix, items[ix].id);
		if ( items[ix].key )
			SetMenuItemCommandKey(menu, ix, false, items[ix].key);
		if ( items[ix].modifier == modNone )
			SetMenuItemModifiers(menu, ix, kMenuNoCommandModifier);
		else if ( items[ix].modifier == modShift )
			SetMenuItemModifiers(menu, ix, kMenuNoCommandModifier | kMenuShiftModifier);
	}
	InsertMenu(menu, 0);
#elif WINDOWS
	HMENU newmenu = CreateMenu();
	for ( int ix = 0; items[ix].id; ++ ix ) {
		if ( ! items[ix].name )
			//This is just a separator line
			AppendMenu(newmenu, MF_SEPARATOR, UINT_PTR(newmenu), "-");
		else {
			string MenuText(items[ix].name);
			if ( items[ix].key ) {
				//It has a shortcut key, so tell people what it is
				MenuText.append("\t");
				switch ( items[ix].modifier ) {
					case modShift: MenuText.append("Shift-"); break;
					case modDefault: MenuText.append("Ctrl-"); break;
					default: break;
				}
				string givenkey(1,items[ix].key);
				string key = "";
				switch ( items[ix].key ) {
					case '\r': key = "Enter"; break;
					case '\t': key = "Tab"; break;
					case VK_PRIOR: key = "PgUp"; break;
					case VK_NEXT: key = "PgDown"; break;
					case VK_HOME: key = "Home"; break;
					case VK_F1: key = "F1"; break;
					case VK_F12: key = "F12"; break;
					case VK_DELETE: key = "Del"; break;
					default: key.append(givenkey); break;
				}
				MenuText.append(key);
				//And add it to the accelerator table
				accelKeys[numAccelKeys].fVirt = FVIRTKEY
					| (items[ix].modifier == modDefault ? FCONTROL : 0)
					| (items[ix].modifier == modShift ? FSHIFT : 0);
				accelKeys[numAccelKeys].cmd = items[ix].id;
				accelKeys[numAccelKeys].key = items[ix].key;
				++numAccelKeys;
			}
			//Now add the MenuItem to the submenu, or the submenu to the main menu
			if (ix)
				AppendMenu(newmenu, MF_STRING, items[ix].id, (char*)MenuText.c_str());
			else
				AppendMenu(mainMenu, MF_POPUP, UINT_PTR(newmenu), items[ix].name);
		}
	}
#else
	//TODO
#endif
}

#if MAC
bool GBViewsApplication::DoNumberDialog(ConstStr255Param prompt, long & value, long min, long max) {
	short itemHit, itemType;
	bool done = false;
	ParamText(prompt, nil, nil, nil);
	DialogPtr dlog = GetNewDialog(kNumberDialogID, nil, (WindowPtr)(-1));
	Handle textBox;
	Rect bounds;
	GetDialogItem(dlog, 4, &itemType, &textBox, &bounds);
	Str255 text;
	NumToString(value, text);
	SetDialogItemText(textBox, text);
	SelectDialogItemText(dlog, 4, 0, -1);
	SetDialogDefaultItem(dlog, 1);
	SetDialogCancelItem(dlog, 2);
	do {
		ModalDialog(nil, &itemHit);
		switch ( itemHit ) {
			case 1: { // ok
				long num;
				GetDialogItemText(textBox, text);
				StringToNum(text, &num);
				if ( num >= min && num <= max ) {
					value = num;
					done = true;
				} else {
					SelectDialogItemText(dlog, 4, 0, -1);
					SysBeep(1);
				}
			} break;
			case 2: // cancel
				done = true; break;
			default: break;
		}
	} while ( ! done );
	DisposeDialog(dlog);
	return itemHit == 1;
}

void GBViewsApplication::SetCursor(GBCursor curs) {
	CursHandle cursor = nil;
	switch ( curs ) {
		case cuArrow: default: break;
		case cuHand: cursor = GetCursor(-20709); break; // might be hand cursor, might not
		case cuCross: cursor = GetCursor(crossCursor); break;
		case cuWait: cursor = GetCursor(watchCursor); break;
	}
	if ( cursor ) ::SetCursor(*cursor);
	else InitCursor();
}
#endif

void GBViewsApplication::AdjustMenus() {} // override

void GBViewsApplication::HandleMenuSelection(int /*item*/) {} // override

void GBViewsApplication::Process() {} // override

void GBViewsApplication::Redraw() {} // override

#if MAC
//return sleep time for WNE, in ticks
long GBViewsApplication::SleepTime() {
	if ( stepPeriod <= 0 ) return stepPeriod;
	GBMilliseconds delay = stepPeriod + lastStep - Milliseconds();
	return delay < 0 ? 0 : (long)((delay * 60 + 30) / 1000);
}

void GBViewsApplication::OpenApp() {} // override if you want to do anything
void GBViewsApplication::OpenFile(FSSpec &) {} // override if you want to do anything
void GBViewsApplication::PrintFile(FSSpec &) {} // override if you want to do anything
#endif

void GBViewsApplication::Quit() {
	// override if you want to do anything different
	alive = false;
}
