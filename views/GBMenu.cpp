#ifdef WITH_SDL
#include "GBMenu.h"
#include "GBSDLApplication.h"

GBMenuView::GBMenuView(GBWorld& world, GBSDLApplication& app)
	: GBView()
	, world(world)
	, app(app)
{}

GBMenuView::~GBMenuView() {}

void GBMenuView::Draw() {
	DrawBackground(GBColor::black);
	
	// pause
	DrawSolidRect(GBRect(2, 2, 6, 12), GBColor::white);
	DrawSolidRect(GBRect(8, 2,12, 12), GBColor::white);
	unsigned char i = 0;
	// minimap
	DrawStringCentered("M", (++i * 16) + 8, 16, 14, GBColor::white);
	// debugger
	DrawStringCentered("D", (++i * 16) + 8, 16, 14, GBColor::white);
	// side debugger
	DrawStringCentered("SD", (++i * 16) + 8, 16, 14, GBColor::white);
	// roster
	DrawStringCentered("R", (++i * 16) + 8, 16, 14, GBColor::white);
	// scores
	DrawStringCentered("S", (++i * 16) + 8, 16, 14, GBColor::white);
	// type
	DrawStringCentered("Tp", (++i * 16) + 8, 16, 14, GBColor::white);
	// tournament
	DrawStringCentered("T", (++i * 16) + 8, 16, 14, GBColor::white);
	// about
	DrawStringCentered("?", (++i * 16) + 8, 16, 14, GBColor::white);
}

short GBMenuView::PreferredWidth() const {
	return 260;
}

short GBMenuView::PreferredHeight() const {
	return 16;
}

const string GBMenuView::Name() const {
	return "Grobots Menu";
}

void GBMenuView::AcceptClick(short x, short y, int /*clicks*/) {
	if (x > (16 * 9)) return;
	switch(x / 16) {
		case 0:
			world.running = !world.running; // pause
			break;
		case 1:
			app.OpenMinimap();
			break;
		case 2:
			app.OpenDebugger();
			break;
		case 3:
			app.OpenSideDebugger();
			break;
		case 4:
			app.OpenRoster();
			break;
		case 5:
			app.OpenScores();
			break;
		case 6:
			app.OpenTypeWindow();
			break;
		case 7:
			app.OpenTournament();
			break;
		case 8:
			app.OpenAbout();
			break;
	}
}
#endif
