// GBGraphics.h
// portable graphics layer
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef _GBGraphics_h
#define _GBGraphics_h

#include "GBPlatform.h"

#ifdef WITH_SDL
  #include <SDL.h>
  #include <SDL2_gfxPrimitives.h>
  #include "GBFontManager.h"
#elif HEADLESS
	//nothing
#elif MAC
	#include <Carbon/Carbon.h>
#elif WINDOWS
	#include <Windows.h>
	#undef SendMessage
	//this macro was clobbering GBSide::SendMessage
#endif

#include "GBColor.h"
#include <string>
using std::string;

class GBRect {
public:
	short left, top, right, bottom;
	GBRect();
	GBRect(short l, short t, short r, short b);
	short Width() const;
	short Height() const;
	short CenterX() const;
	short CenterY() const;
	void Shrink(short step);
	void Clip(const GBRect & r);
#ifdef WITH_SDL
	void ToRect(SDL_Rect & r) const;
	GBRect(SDL_Rect & r);
#elif MAC && ! HEADLESS
	void ToRect(Rect & r) const;
	GBRect(Rect & r);
#elif WINDOWS
	void ToRect(RECT & r) const;
#endif
};

class GBBitmap;

class GBGraphics {
#ifdef WITH_SDL
	SDL_Surface* surf;
	GBFontManager* font_mgr;
  SDL_Renderer* renderer;
public:
	GBGraphics(SDL_Surface * surf, GBFontManager * font_mgr);
	void setSurface(SDL_Surface* surf);
	friend class GBBitmap;
#elif HEADLESS
public:
	GBGraphics();
#elif MAC
	// no data members
	void UseColor(const GBColor & c);
public:
	GBGraphics();
#elif WINDOWS
	HDC hdc;
	static COLORREF ColorRef(const GBColor & c);
	void DrawString(const string & str, short x, short y,
		short size, const GBColor & color, bool bold);
public:
	GBGraphics(HDC dc);
	friend class GBBitmap;
#else
	#warning "Need GBGraphics."
#endif
public:
	~GBGraphics();
// lines
	void DrawLine(short x1, short y1, short x2, short y2,
		const GBColor & color, short thickness = 1);
// rectangles
	void DrawSolidRect(const GBRect & where, const GBColor & color);
	void DrawOpenRect(const GBRect & where, const GBColor & color, short thickness = 1);
// ovals
	void DrawSolidOval(const GBRect & where, const GBColor & color);
	void DrawOpenOval(const GBRect & where, const GBColor & color, short thickness = 1);
	void DrawArc(const GBRect & where, short startAngle, short length,
		const GBColor & color, short thickness = 1);
// strings
	void DrawStringLeft(const string & str, short x, short y,
		short size, const GBColor & color = GBColor::black, bool bold = false);
	void DrawStringRight(const string & str, short x, short y,
		short size, const GBColor & color = GBColor::black, bool bold = false);
	void DrawStringCentered(const string & str, short x, short y,
		short size, const GBColor & color = GBColor::black, bool bold = false);
	void DrawStringPair(const string & str1, const string & str2,
		short left, short right, short y, short size, const GBColor & color, bool useBold);

// blitter
	void Blit(const GBBitmap & src, const GBRect & srcRect, const GBRect & destRect);
};

class GBBitmap {
private:
#ifdef WITH_SDL
	SDL_Surface* surf;
	friend class GBGraphics;
#elif HEADLESS
#elif MAC
	GWorldPtr world;
	CGrafPtr savePort;
	GDHandle saveDevice;
public:
	BitMapPtr Bits() const;
#elif WINDOWS
	HBITMAP bits;
	HDC hdc;
#else
	#warning "Need GBBitmap."
#endif
private:
	GBRect bounds;
	GBGraphics graphics;
public:
	GBBitmap(short width, short height, GBGraphics & parent);
	~GBBitmap();
	const GBRect & Bounds() const;
	GBGraphics & Graphics();
	const GBGraphics & Graphics() const;
// must call StartDrawing and StopDrawing around any drawing to the bitmap,
//  to allow saving and restoring state :(
	void StartDrawing();
	void StopDrawing();
};

#endif // GBGraphics_h
