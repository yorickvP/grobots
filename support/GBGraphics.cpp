// GBGraphics.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBGraphics.h"
#include "GBStringUtilities.h"
#include <math.h>
#ifdef WITH_SDL
#include "SDL2_gfxPrimitives.h"
#include "BBCSDL_gfx.h"
#endif

// GBRect //

GBRect::GBRect() : left(0), top(0), right(0), bottom(0) {}

GBRect::GBRect(short l, short t, short r, short b)
	: left(l), top(t), right(r), bottom(b) {}

short GBRect::Width() const { return right - left; }
short GBRect::Height() const { return bottom - top; }
short GBRect::CenterX() const { return (right + left) >> 1; }
short GBRect::CenterY() const { return (top + bottom) >> 1; }

void GBRect::Shrink(short step) {
	left += step;
	top += step;
	right -= step;
	bottom -= step;
}
void GBRect::Clip(const GBRect & r) {
	left = min(max(left, r.left), r.right);
	top = min(max(top, r.top), r.bottom);
	right = min(right, r.right);
	bottom = min(bottom, r.bottom);
}

bool GBRect::HasPoint(short x, short y) const {
  return left <= x && x <= right && top <= y && y <= bottom;
}

void GBRect::SetXY(const short x, const short y) {
  right -= left - x;
  bottom -= top - y;
  left = x;
  top = y;
}
#ifdef WITH_SDL
void GBRect::ToRect(SDL_Rect & r) const {
	r.x = left;
	r.y = top;
	r.w = right-left;
	r.h = bottom-top;
}
GBRect::GBRect(SDL_Rect & r)
	: left(r.x), top(r.y), right(r.x + r.w), bottom(r.y + r.h) {}

#elif MAC && ! HEADLESS
void GBRect::ToRect(Rect & r) const {
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;
}

GBRect::GBRect(Rect & r)
	: left(r.left), top(r.top), right(r.right), bottom(r.bottom) {}
#elif WINDOWS && ! HEADLESS
void GBRect::ToRect(RECT & r) const {
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;
}
#endif

// GBGraphics //
#ifdef WITH_SDL

GBGraphics::GBGraphics(SDL_Renderer * renderer, GBFontManager* font_mgr) : renderer(renderer), font_mgr(font_mgr) {
};
GBGraphics::~GBGraphics() {}
void GBGraphics::DrawLine(short x1, short y1, short x2, short y2,
		const GBColor & color, short thickness) {
	if (renderer == nil) return;
	thickLineRGBA(renderer, x1, y1, x2, y2, thickness, color.Red() * 255, color.Green() * 255, color.Blue() * 255, 255);
}
void GBGraphics::DrawSolidRect(const GBRect & where, const GBColor & color) {
	if (renderer == nil) return;
  SDL_Rect r1;
  where.ToRect(r1);
  SDL_SetRenderDrawColor(renderer, color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255);
  SDL_RenderFillRect(renderer, &r1);
}
void GBGraphics::DrawOpenRect(const GBRect & where, const GBColor & color, short thickness) {
	if (renderer == nil) return;
  SDL_Rect r1;
  where.ToRect(r1);
  SDL_SetRenderDrawColor(renderer, color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255);
  SDL_RenderDrawRect(renderer, &r1);
}
void GBGraphics::DrawSolidOval(const GBRect & r, const GBColor & color) {
	if (renderer == nil) return;
	SDL_Rect r1;
	r.ToRect(r1);
	filledEllipseRGBA(renderer, r1.x + (r1.w/2), r1.y + (r1.h/2), r1.w/2, r1.h/2, color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255);
}
void GBGraphics::DrawOpenOval(const GBRect & r, const GBColor & color, short thickness) {
	if (renderer == nil) return;
	SDL_Rect r1;
	r.ToRect(r1);
	thickEllipseRGBA(renderer, r1.x + (r1.w/2), r1.y + (r1.h/2), r1.w/2, r1.h/2, color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255, thickness);
}
//startAngle: degrees clockwise from up
//length: degrees
void GBGraphics::DrawArc(const GBRect & where, short startAngle, short length,
		const GBColor & color, short thickness) {
  thickArcRGBA(renderer, where.CenterX(), where.CenterY(), where.Width()/2, startAngle, startAngle + length, color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255, thickness);
}
void GBGraphics::DrawStringLeft(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	if (renderer == nil) return;
	SDL_Rect destrect = {x, y, 0, 0};
  GBFontManager::Text text = font_mgr->renderText_Blended(renderer, size, useBold, str, color);
	destrect.w = text->w;
	destrect.h = text->h;
	destrect.y -= text->h;
  text->draw(renderer, nil, &destrect);
	//stringRGBA(renderer, x, y, str.c_str(), color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255);
}
void GBGraphics::DrawStringCentered(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	if (renderer == nil) return;
	SDL_Rect destrect = {x, y, 0, 0};
  GBFontManager::Text text = font_mgr->renderText_Blended(renderer, size, useBold, str, color);
	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x -= text->w / 2;
	destrect.y -= text->h;
  text->draw(renderer, nil, &destrect);
	//stringRGBA(renderer, x - (str.length() * 4), y, str.c_str(), color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255);
}
void GBGraphics::DrawStringRight(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	if (renderer == nil) return;
	SDL_Rect destrect = {x, y, 0, 0};
  GBFontManager::Text text = font_mgr->renderText_Blended(renderer, size, useBold, str, color);
	destrect.w = text->w;
	destrect.h = text->h;
	destrect.x -= text->w;
	destrect.y -= text->h;
  text->draw(renderer, nil, &destrect);
	//stringRGBA(renderer, x - str.length() * 8, y, str.c_str(), color.Red()*0xFF, color.Green()*0xFF, color.Blue()*0xFF, 255);
}

GBRect GBGraphics::MeasureText(const string & str, short size, const GBColor & color, bool useBold) {
  GBFontManager::Text text = font_mgr->renderText_Blended(renderer, size, useBold, str, color);
  return GBRect(0, 0, text->w, text->h);
}
void GBGraphics::Blit(const GBBitmap & src, const GBRect & srcRect, const GBRect & destRect, unsigned char alpha) {
	SDL_Rect r1, r2;
	srcRect.ToRect(r1);
	destRect.ToRect(r2);

  if (alpha != 255) {
    SDL_SetTextureBlendMode(src.texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(src.texture, alpha);
    SDL_RenderCopy(renderer, src.texture, &r1, &r2);
    SDL_SetTextureBlendMode(src.texture, SDL_BLENDMODE_NONE);
    //SDL_SetTextureAlphaMod(src.texture, 255);
  } else {
    SDL_RenderCopy(renderer, src.texture, &r1, &r2);
  }
}
#elif HEADLESS
GBGraphics::GBGraphics() {}
GBGraphics::~GBGraphics() {}
void GBGraphics::DrawLine(short, short, short, short, const GBColor &, short) {}
void GBGraphics::DrawSolidRect(const GBRect &, const GBColor &) {}
void GBGraphics::DrawOpenRect(const GBRect &, const GBColor &, short) {}

void GBGraphics::DrawSolidOval(const GBRect &, const GBColor &) {}
void GBGraphics::DrawOpenOval(const GBRect &, const GBColor &, short) {}
void GBGraphics::DrawArc(const GBRect &, short, short, const GBColor &, short) {}

void GBGraphics::DrawStringLeft(const string &, short, short, short, const GBColor &, bool) {}
void GBGraphics::DrawStringRight(const string &, short, short, short, const GBColor &, bool) {}
void GBGraphics::DrawStringCentered(const string &, short, short, short, const GBColor &, bool) {}

void GBGraphics::Blit(const GBBitmap &, const GBRect &, const GBRect &, const unsigned char) {}
#endif
#if HEADLESS || defined(WITH_SDL)


#elif MAC
void GBGraphics::UseColor(const GBColor & c) {
	RGBColor color;
	color.red = 0xFFFF * c.Red();
	color.green = 0xFFFF * c.Green();
	color.blue = 0xFFFF * c.Blue();
	RGBForeColor(&color);
}

GBGraphics::GBGraphics() {}
GBGraphics::~GBGraphics() {}

void GBGraphics::DrawLine(short x1, short y1, short x2, short y2,
		const GBColor & color, short thickness) {
//correct for pen drawing down and to the right
	x1 -= thickness >> 1;
	x2 -= thickness >> 1;
	y1 -= thickness >> 1;
	y2 -= thickness >> 1;
//draw
	UseColor(color);
	PenSize(thickness, thickness);
	MoveTo(x1, y1);
	LineTo(x2, y2);
}

void GBGraphics::DrawSolidRect(const GBRect & where, const GBColor & color) {
	Rect r;
	where.ToRect(r);
	UseColor(color);
	PaintRect(&r);
}

void GBGraphics::DrawOpenRect(const GBRect & where, const GBColor & color, short thickness) {
	Rect r;
	where.ToRect(r);
	UseColor(color);
	PenSize(thickness, thickness);
	FrameRect(&r);
}

void GBGraphics::DrawSolidOval(const GBRect & where, const GBColor & color) {
	Rect r;
	where.ToRect(r);
	UseColor(color);
	PaintOval(&r);
}

void GBGraphics::DrawOpenOval(const GBRect & where, const GBColor & color, short thickness) {
	Rect r;
	where.ToRect(r);
	UseColor(color);
	PenSize(thickness, thickness);
	FrameOval(&r);
}

//startAngle: degrees clockwise from up
//length: degrees
void GBGraphics::DrawArc(const GBRect & where, short startAngle, short length,
		const GBColor & color, short thickness) {
	Rect r;
	where.ToRect(r);
	UseColor(color);
	PenSize(thickness, thickness);
	FrameArc(&r, startAngle, length);
}

void GBGraphics::DrawStringLeft(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	Str255 s;
	ToPascalString(str, s);
	UseColor(color);
	TextSize(size);
	TextFace(useBold ? bold : normal);
	MoveTo(x, y);
	DrawString(s);
}

void GBGraphics::DrawStringRight(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	Str255 s;
	ToPascalString(str, s);
	UseColor(color);
	TextSize(size);
	TextFace(useBold ? bold : normal);
	MoveTo(x - StringWidth(s), y);
	DrawString(s);
}

void GBGraphics::DrawStringCentered(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	Str255 s;
	ToPascalString(str, s);
	UseColor(color);
	TextSize(size);
	TextFace(useBold ? bold : normal);
	MoveTo(x - StringWidth(s) / 2, y);
	DrawString(s);
}

void GBGraphics::Blit(const GBBitmap & src, const GBRect & srcRect, const GBRect & destRect, unsigned char /*alpha*/) {
	Rect r1, r2;
	srcRect.ToRect(r1);
	destRect.ToRect(r2);
	GrafPtr port;
	GetPort(&port);
	ForeColor(blackColor);
	CopyBits(src.Bits(), GetPortBitMapForCopyBits(port), &r1, &r2, srcCopy, nil);
}

#elif WINDOWS

COLORREF GBGraphics::ColorRef(const GBColor & c) {
	return RGB(255 * c.Red(), 255 * c.Green(), 255 * c.Blue());
}

GBGraphics::GBGraphics(HDC dc) : hdc(dc) {
	SetBkMode(hdc, TRANSPARENT);
}

GBGraphics::~GBGraphics() {}

void GBGraphics::DrawLine(short x1, short y1, short x2, short y2,
		const GBColor & color, short thickness) {
	HPEN pen = CreatePen(PS_SOLID, thickness, ColorRef(color));
	HGDIOBJ old = SelectObject(hdc, pen);
	MoveToEx(hdc, x1, y1, 0);
	LineTo(hdc, x2, y2);
	SelectObject(hdc, old);
	DeleteObject(pen);
}

void GBGraphics::DrawSolidRect(const GBRect & r, const GBColor & color) {
	HBRUSH brush = CreateSolidBrush(ColorRef(color));
	RECT rect;
	r.ToRect(rect);
	FillRect(hdc, &rect, brush);
	DeleteObject(brush);
}

void GBGraphics::DrawOpenRect(const GBRect & r, const GBColor & color, short thickness) {
	HBRUSH brush = CreateSolidBrush(ColorRef(color));
	RECT rect;
	r.ToRect(rect);
	FrameRect(hdc, &rect, brush);
	DeleteObject(brush);
}

void GBGraphics::DrawSolidOval(const GBRect & r, const GBColor & color) {
	HBRUSH brush = CreateSolidBrush(ColorRef(color));
	HPEN pen = CreatePen(PS_SOLID, 0, ColorRef(color));
	HGDIOBJ oldbrush = SelectObject(hdc, brush);
	HGDIOBJ oldpen = SelectObject(hdc, pen);
	Ellipse(hdc, r.left, r.top, r.right, r.bottom);
	SelectObject(hdc, oldpen);
	SelectObject(hdc, oldbrush);
	DeleteObject(brush);
	DeleteObject(pen);
}

void GBGraphics::DrawOpenOval(const GBRect & r, const GBColor & color, short thickness) {
	DrawArc(r, 180, 360, color, thickness);
}

void GBGraphics::DrawArc(const GBRect & r, short startAngle, short length,
						 const GBColor & color, short thickness) {
	short start = 90 - startAngle - length; //different direction
	const float kPiOver180 = 3.14159265f / 180;
	HPEN pen = CreatePen(PS_SOLID, thickness, ColorRef(color));
	HGDIOBJ old = SelectObject(hdc, pen);
	MoveToEx(hdc, r.CenterX() + r.Width() * cos(start * kPiOver180) / 2,
		r.CenterY() + r.Height() * sin(start * kPiOver180) / -2, 0);
	AngleArc(hdc, r.CenterX(), r.CenterY(), r.Height() / 2, start, length);
	SelectObject(hdc, old);
	DeleteObject(pen);
}

void GBGraphics::DrawString(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	HFONT f = CreateFont(- size, 0, 0, 0, useBold ? FW_BOLD : FW_NORMAL,
		0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	HGDIOBJ old = SelectObject(hdc, f);
	SetTextColor(hdc, ColorRef(color));
	TextOut(hdc, x, y, str.c_str(), str.length());
	SelectObject(hdc, old);
	DeleteObject(f);
}

void GBGraphics::DrawStringLeft(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	SetTextAlign(hdc, TA_LEFT | TA_BASELINE);
	DrawString(str, x, y, size, color, useBold);
}

void GBGraphics::DrawStringRight(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	SetTextAlign(hdc, TA_RIGHT | TA_BASELINE);
	DrawString(str, x, y, size, color, useBold);
}

void GBGraphics::DrawStringCentered(const string & str, short x, short y,
		short size, const GBColor & color, bool useBold) {
	SetTextAlign(hdc, TA_CENTER | TA_BASELINE);
	DrawString(str, x, y, size, color, useBold);
}

void GBGraphics::Blit(const GBBitmap & src, const GBRect & srcRect, const GBRect & destRect, unsigned char /*alpha*/) {
	if (!BitBlt(hdc, destRect.left, destRect.top, destRect.Width(), destRect.Height(),
			src.hdc, srcRect.left, srcRect.top, SRCCOPY))
		DrawSolidRect(destRect, GBColor::black);
}


#else
	#warning "Need implementation of GBGraphics."
#endif

void GBGraphics::DrawStringPair(const string & str1, const string & str2,
		short left, short right, short y, short size, const GBColor & color, bool useBold) {
	DrawStringLeft(str1, left, y, size, color, useBold);
	DrawStringRight(str2, right, y, size, color, useBold);
}



// GBBitmap //

const GBRect & GBBitmap::Bounds() const {
	return bounds;}

GBGraphicsWrapper GBBitmap::Graphics() {
	return GBGraphicsWrapper(*this, graphics);}

void GBBitmap::SetPosition(short x, short y) {
  bounds.SetXY(x, y);
}
	
#ifdef WITH_SDL
SDL_Surface* CreateCompatibleRGBSurface(Uint32 flags, short width, short height) {
    Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    //const SDL_PixelFormat& fmt = *(source->format);
	return SDL_CreateRGBSurface(flags, width, height, 32, rmask, gmask, bmask, amask);
}
GBBitmap::GBBitmap(short width, short height, GBGraphics &g)
	: // todo: getwindowpixelformat
  texture(SDL_CreateTexture(g.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height)),
  renderer(g.renderer),
  bounds(0, 0, width, height), graphics(renderer, g.font_mgr)
{}

GBBitmap::~GBBitmap() { SDL_DestroyTexture(texture); }

void GBBitmap::StartDrawing() {
  saveTexture = SDL_GetRenderTarget(renderer);
  SDL_SetRenderTarget(renderer, texture);
}
void GBBitmap::StopDrawing() {
  SDL_SetRenderTarget(renderer, saveTexture);
  saveTexture = nil;
}
void GBBitmap::SetClip(const GBRect* clip) {
  if (clip) {
    SDL_Rect t;
    clip->ToRect(t);
    SDL_RenderSetClipRect(renderer, &t);
  } else {
    SDL_RenderSetClipRect(renderer, nil);
  }
}

#elif HEADLESS
GBBitmap::GBBitmap(short width, short height, GBGraphics &g)
	: bounds(0, 0, width, height), graphics(g)
{}

GBBitmap::~GBBitmap() {}

void GBBitmap::StartDrawing() {}
void GBBitmap::StopDrawing() {}
void GBBitmap::SetClip(const GBRect*) {}

#elif MAC
GBBitmap::GBBitmap(short width, short height, GBGraphics &)
	: bounds(0, 0, width, height),
	world(nil),
	savePort(nil), saveDevice(nil),
	graphics()
{
	Rect r;
	bounds.ToRect(r);
	if ( NewGWorld(&world, 0, &r, nil, nil, 0) )
		throw GBOutOfMemoryError();
}

GBBitmap::~GBBitmap() {
	if ( world ) {
		DisposeGWorld(world);
		world = nil;
	}
}

BitMapPtr GBBitmap::Bits() const {
	return *(BitMapHandle)GetGWorldPixMap(world);
}

void GBBitmap::StartDrawing() {
	GetGWorld(&savePort, &saveDevice);
	SetGWorld(world, nil);
}

void GBBitmap::StopDrawing() {
	SetGWorld(savePort, saveDevice);
}

#elif WINDOWS

GBBitmap::GBBitmap(short width, short height, GBGraphics & parent)
	: bounds(0, 0, width, height),
	bits(CreateCompatibleBitmap(parent.hdc, width, height)),
	graphics(CreateCompatibleDC(parent.hdc))
{
	SelectObject(graphics.hdc, bits);
}

GBBitmap::~GBBitmap() {
	DeleteDC(graphics.hdc);
	DeleteObject(bits);
}

void GBBitmap::StartDrawing() {}
void GBBitmap::StopDrawing() {}
void GBBitmap::SetClip(const GBRect* /*clip*/) {/*todo*/}

#else
	#warning "Need implementation of GBBitmap."
#endif

