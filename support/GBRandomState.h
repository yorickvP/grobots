// GBRandomState.h
// random number generator
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef GBRandomState_h
#define GBRandomState_h

#include "GBTypes.h"
#include "GBColor.h"


class GBRandomState {
	int seed;
public:
	GBRandomState();
	GBRandomState(const unsigned int newseed);
private:
	unsigned int Generate();
public:
	int IntInRange(const int min, const int max);
	GBNumber InRange(const GBNumber min, const GBNumber max);
	float FloatInRange(const float min, const float max);
	GBAngle Angle();
	GBVector Vector(const GBDistance maxLength);
	GBColor Color();
	GBColor ColorNear(const GBColor & old, float dist);
	bool Boolean(const GBNumber probability);
	bool Boolean(const int num, const int denom);
};

extern GBRandomState gRandoms;

#endif
