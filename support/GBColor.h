// GBColor.h
// portable RGB color class
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef GBColor_h
#define GBColor_h

#ifdef WITH_SDL
#include "SDL2/SDL_pixels.h"
#endif
#include <functional>

class GBColor {
	float r, g, b;
public:
	GBColor();
  inline std::size_t hash() const {
    std::size_t seed = 0;
    std::hash<float> hasher;
    seed ^= hasher(r) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    seed ^= hasher(g) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    seed ^= hasher(b) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    return seed;
  };
  bool operator==(const GBColor& other) const = default;
	explicit GBColor(const float grey);
	GBColor(const float red, const float green, const float blue);
	float Red() const { return r; }
	float Green() const { return g; }
	float Blue() const { return b; }
	void Set(const float red, const float green, const float blue);
	float Lightness() const;
// computations
	const GBColor Mix(const float fraction, const GBColor & other) const;
	float Contrast(const GBColor & other) const;
	const GBColor EnsureContrastWithBlack(const float minimum) const;
	const GBColor ChooseContrasting(const GBColor & primary, const GBColor & secondary, const float cutoff) const;
	const GBColor ContrastingTextColor() const;
	const GBColor operator *(float multiplier) const;
	const GBColor operator /(float divisor) const;
	const GBColor operator +(const GBColor & other) const;
// statics
	static float Limit(float val);
// handy constants
	static const GBColor red;
	static const GBColor green;
	static const GBColor blue;
	static const GBColor cyan;
	static const GBColor magenta;
	static const GBColor yellow;
	static const GBColor black;
	static const GBColor white;
	static const GBColor gray;
	static const GBColor darkGray;
	static const GBColor lightGray;
	static const GBColor purple;	
	static const GBColor darkGreen;
	static const GBColor darkRed;
#ifdef WITH_SDL
  explicit operator SDL_Color() const {
    return {(Uint8)(r*0xFF), (Uint8)(g*0xFF), (Uint8)(b*0xFF), 0xFF};
  }
#endif
};

// implementation //

inline GBColor::GBColor()
	: r(1.0), g(1.0), b(1.0)
{}

namespace std {
  template<> struct hash<GBColor> {
    std::size_t operator()(const GBColor &t) const {
      return t.hash();
    };
  };
};

#endif
