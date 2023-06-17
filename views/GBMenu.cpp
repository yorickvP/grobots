#ifdef WITH_SDL
#include "GBMenu.h"
#include "GBSDLApplication.h"

//Menu item IDs: these are 100*menuid + itemposition
enum {
	kFileMenu = 129,
		miLoadSide = 12901, miDuplicateSide,
		miReloadSide = 12903,
		miRemoveSide = 12905, miRemoveAllSides,
		miClose = 12908,
		miQuit = 12910,
	kWindowMenu = 130,
		miAbout = 13001,
		miRosterView, miMainView, miMinimapView,
		miScoresView, miTypesView,
		miDebuggerView,
		miTournamentView, miSideDebuggerView,
	kViewMenu = 131,
		miSound = 13101,
		miZoomIn = 13103, miZoomOut, miZoomStandard,
		miShowSensors = 13107, miShowDecorations, miShowMeters,
		miMinimapTrails,
		miReportErrors = 13112, miReportPrints,
		miRefollow = 13115, miFollowRandom, miRandomNear, miAutofollow,
		miNextPageMemory = 13120, miPreviousPageMemory, miFirstPageMemory,
	kSimulationMenu = 132,
		miRun = 13201, miSingleFrame, miStep, miPause, miStartStopBrain,
		miSlowerSpeed = 13207, miSlowSpeed, miNormalSpeed, miFastSpeed, miFasterSpeed, miUnlimitedSpeed,
		miNewRound = 13214, miRestart,
		miSeed, miReseed,
		miRules = 13219,
	miTournament = 13220, miSaveScoresHtml, miSaveScoresWiki, miResetScores,
	kToolsMenu = 133,
		miScroll = 13301,
		miAddManna = 13303, miAddRobot, miAddSeed,
		miMove = 13307, miPull, miSmite, miBlasts, miErase, miEraseBig
};

GBMenuView::GBMenuView(GBSDLApplication& app, GBFontManager& fontmgr, std::shared_ptr<std::list<MenuItem>> children)
	: GBView()
	, app(app)
  , fontmgr(fontmgr)
  , width(0)
  , isMain(children == nil)
{
  if (children) {
    topMenuItems = children;
  } else {
    topMenuItems = std::make_shared<std::list<MenuItem>>();
    MenuItem* f;
    f = &topMenuItems->emplace_back("File");
    f->children->emplace_back("Open Side", miLoadSide);
    f->children->emplace_back("Duplicate Side", miDuplicateSide);
    f->children->emplace_back("Reload Side", miReloadSide);
    f->children->emplace_back("");
    f->children->emplace_back("Remove Side", miRemoveSide);
    f->children->emplace_back("Remove All Sides", miRemoveAllSides);
    f->children->emplace_back("");
    f->children->emplace_back("Exit", miQuit);

    f = &topMenuItems->emplace_back("Window");
    f->children->emplace_back("&Roster", miRosterView);
    f->children->emplace_back("&Minimap", miMinimapView);
    f->children->emplace_back("&Statistics", miScoresView);
    f->children->emplace_back("T&ypes", miTypesView);
    f->children->emplace_back("&Debugger", miDebuggerView);
    f->children->emplace_back("&Tournament", miTournamentView);
    f->children->emplace_back("Shared Memory", miSideDebuggerView);

    f = &topMenuItems->emplace_back("View");
    f->children->emplace_back("Sound", miSound); // GRAYED
    f->children->emplace_back("");
    f->children->emplace_back("Show &Sensors", miShowSensors);
    f->children->emplace_back("Show &Decorations", miShowDecorations); // CHECKED
    f->children->emplace_back("Show &Meters", miShowMeters); // CHECKED
    f->children->emplace_back("Minimap Trails", miMinimapTrails);
    f->children->emplace_back("");
    f->children->emplace_back("Report &Errors", miReportErrors); // CHECKED
    f->children->emplace_back("Report &Prints", miReportPrints);
    f->children->emplace_back("");
    f->children->emplace_back("Refollow", miRefollow);
    f->children->emplace_back("Follow Random", miFollowRandom);
    f->children->emplace_back("Follow Near", miRandomNear);
    f->children->emplace_back("&Autofollow", miAutofollow);
    f->children->emplace_back("");
    f->children->emplace_back("&Graph All Rounds", 13121);

    f = &topMenuItems->emplace_back("Simulation");
    f->children->emplace_back("&Run", miRun);
    f->children->emplace_back("Single &Frame", miSingleFrame);
    f->children->emplace_back("Step", miStep); // GRAYED
    f->children->emplace_back("&Pause", miPause);
    f->children->emplace_back("Start/Stop &Brain", miStartStopBrain);
    f->children->emplace_back("");
    f->children->emplace_back("Slowest (2 fps)", miSlowerSpeed);
    f->children->emplace_back("Slow (&10 fps)", miSlowSpeed);
    f->children->emplace_back("Normal (&30 fps)", miNormalSpeed); // CHECKED
    f->children->emplace_back("Fast (&60 fps)", miFastSpeed);
    f->children->emplace_back("Faster (300 fps)", miFasterSpeed);
    f->children->emplace_back("&Unlimited", miUnlimitedSpeed);

    f = &topMenuItems->emplace_back("Rounds");
    f->children->emplace_back("&New Round", miNewRound);
    f->children->emplace_back("Clear World", miRestart);
    f->children->emplace_back("Add Seeds", miSeed);
    f->children->emplace_back("Reseed Dead Sides", miReseed);
    f->children->emplace_back("");
    f->children->emplace_back("Rules", miRules); // GRAYED
    f->children->emplace_back("&Tournament", miTournament);
    f->children->emplace_back("Sa&ve Scores (HTML)", miSaveScoresHtml);
    f->children->emplace_back("Sa&ve Scores (wiki)", miSaveScoresWiki);
    f->children->emplace_back("Reset Sc&ores", miResetScores);

    f = &topMenuItems->emplace_back("Tools", 13300);
    f->children->emplace_back("Scroll", miScroll);
    f->children->emplace_back("");
    f->children->emplace_back("Add Manna", miAddManna);
    f->children->emplace_back("Add Robot", miAddRobot);
    f->children->emplace_back("Add Seed", miAddSeed);
    f->children->emplace_back("");
    f->children->emplace_back("Move", miMove);
    f->children->emplace_back("Pull", miPull);
    f->children->emplace_back("Smite", miSmite);
    f->children->emplace_back("Blasts", miBlasts);
    f->children->emplace_back("Erase", miErase);
    f->children->emplace_back("Erase Area", miEraseBig);

    f = &topMenuItems->emplace_back("Help", 12800);
    f->children->emplace_back("&About Grobots", miAbout);
  }
}


GBMenuView::~GBMenuView() {}

void GBMenuView::Draw() {
	DrawBackground(GBColor::black);

  width = 10;
  for(auto &item : *topMenuItems) {
    item.x = width;
    // separator
    if (item.name.length() == 0) {
      DrawLine(width, 0, width, 16, GBColor::white, 1);
      width += 11;
      continue;
    }
    DrawStringLeft(item.name, width, 16, 14, GBColor::white);
    width += Graphics().MeasureText(item.name, 14, GBColor::white).Width() + 10;
  }
}

short GBMenuView::PreferredWidth() const {
	return width == 0 ? 260 : width;
}

short GBMenuView::MinimumWidth() const {
	return width == 0 ? 260 : width;
}

short GBMenuView::PreferredHeight() const {
	return 16;
}

const string GBMenuView::Name() const {
	return "";
}

void GBMenuView::AcceptClick(short x, short y, int /*clicks*/) {
  if (!width) return; // need draw first
  if (y > PreferredHeight()) return;
  for(auto it = topMenuItems->begin(); it != topMenuItems->end(); it++) {
    auto nx = std::next(it);
    if (x > it->x && (nx == topMenuItems->end() || x < nx->x)) {
      if (!it->children->empty()) {
        app.OpenView(std::make_shared<GBMenuView>(app, fontmgr, it->children), -1, PreferredHeight());
      } else {
        if (it->id != 0) {
          app.HandleMenuSelection(it->id);
        }
      }
      return;
    }
  }
}

void GBMenuView::SetFocus(bool focus) {
  if (!isMain && !focus) app.CloseView(*this);
}

bool GBMenuView::GetFrontClicks() const {
  return true;
}
#endif
