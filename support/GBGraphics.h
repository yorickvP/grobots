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
	#include <windows.h>
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
  bool HasPoint(short x, short y) const;
  void SetXY(const short x, const short y);
#ifdef WITH_SDL
	void ToRect(SDL_Rect & r) const;
	GBRect(SDL_Rect & r);
#elif MAC && ! HEADLESS
	void ToRect(Rect & r) const;
	GBRect(Rect & r);
#elif WINDOWS && ! HEADLESS
	void ToRect(RECT & r) const;
#endif
};

class GBBitmap;
class GBGraphics;

class GBClip {
  GBRect oldclip;
  GBGraphics* parent;
public:
  GBClip(GBRect& oldclip, GBGraphics* parent) : oldclip(oldclip), parent(parent) {};
  ~GBClip();
  GBClip(const GBClip&) = delete;
  GBClip(GBClip&&) = delete;
};

class GBGraphics {
#ifdef WITH_SDL
protected:
  SDL_Renderer* renderer;
	GBFontManager* font_mgr;
public:
	GBGraphics(SDL_Renderer * renderer, GBFontManager * font_mgr);
	friend class GBBitmap;
	friend class GBClip;
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

  GBRect MeasureText(const string & str, short size, const GBColor & color = GBColor::black, bool bold = false);

// blitter
	void Blit(const GBBitmap & src, const GBRect & srcRect, const GBRect & destRect, unsigned char alpha=255);
  [[nodiscard]] GBClip SetClip(const GBRect* clip);
};
class GBGraphicsWrapper;
class GBBitmap {
  friend class GBGraphicsWrapper;
private:
#ifdef WITH_SDL
  SDL_Texture* texture;
  SDL_Renderer* renderer;
  SDL_Texture* saveTexture;
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
public:
  HDC GetHDC() const;
#else
	#warning "Need GBBitmap."
#endif
private:
	GBRect bounds;
	GBGraphics graphics;
protected:
// must call StartDrawing and StopDrawing around any drawing to the bitmap,
//  to allow saving and restoring state :(
	void StartDrawing();
	void StopDrawing();
public:
	GBBitmap(short width, short height, GBGraphics & parent);
  GBBitmap(const GBBitmap&) = delete;
	~GBBitmap();
	const GBRect & Bounds() const;
  GBGraphicsWrapper Graphics();
  void SetPosition(short x, short y);
};

class GBGraphicsWrapper {
  GBBitmap& b;
  GBGraphics& g;
  GBClip c;
public:
  GBGraphicsWrapper(GBBitmap& b, GBGraphics& g) : b(b), g(g), c(g.SetClip(nil)) {
    b.StartDrawing();
  }
  GBGraphicsWrapper(const GBGraphicsWrapper&) = delete;
  ~GBGraphicsWrapper() {
    b.StopDrawing();
  }
  GBGraphics* operator->() { return &g; };
  // todo: evil
  GBGraphics* operator*() { return &g; };
};

#endif // GBGraphics_h
