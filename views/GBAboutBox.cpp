// GBAboutBox.cpp
// Grobots (c) 2002-2012 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBAboutBox.h"
#include "GBColor.h"


GBAboutBox::GBAboutBox()
	: GBView()
{}

GBAboutBox::~GBAboutBox() {}

void GBAboutBox::Draw() {
	DrawBackground(GBColor::black);
	DrawStringCentered("Grobots", Width() / 2, 50, 40, GBColor::green, true);
	DrawStringCentered("by Devon and Warren Schudy", Width() / 2, 75, 12, GBColor::white);
	DrawStringCentered("built " __DATE__
		#if CARBON
			" for Mac (Carbon)"
		#elif WINDOWS
			" for Win32"
    #elif __EMSCRIPTEN__
      " for WebAssembly"
		#elif LINUX
			" for Linux"
		#endif
		, Width() / 2, 95, 10, GBColor::magenta);
	DrawStringLeft("Additional contributors:", 15, 115, 10, GBColor::white);
	DrawStringLeft("Tilendor", 35, 128, 10, GBColor::white);
	DrawStringLeft("Daniel von Fange", 35, 138, 10, GBColor::white);
	DrawStringLeft("Borg", 35, 148, 10, GBColor::white);
	DrawStringLeft("Eugen Zagorodniy", 35, 158, 10, GBColor::white);
	DrawStringLeft("Rick Manning", 35, 168, 10, GBColor::white);
	DrawStringLeft("Mike Anderson", 35, 178, 10, GBColor::white);
	DrawStringCentered("http://grobots.sourceforge.net/", Width() / 2, 205, 10, GBColor(0, 0.7f, 1));
	DrawStringLeft("Grobots comes with ABSOLUTELY NO WARRANTY.", 10, 228, 10, GBColor::white);
    DrawStringLeft("This is free software, and you are welcome to", 10, 240, 10, GBColor::white);
	DrawStringLeft("redistribute it under the GNU General Public License.", 10, 252, 10, GBColor::white);
}

short GBAboutBox::PreferredWidth() const {
	return 290;
}

short GBAboutBox::PreferredHeight() const {
	return 265;
}

const string GBAboutBox::Name() const {
	return "About Grobots";
}

