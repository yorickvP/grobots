#ifdef WITH_SDL
#include "GBMenu.h"
#include "GBSDLApplication.h"

GBMenuView::GBMenuView(GBSDLApplication& app, GBFontManager& fontmgr, std::list<MenuItem>* children)
	: GBView()
	, app(app)
  , fontmgr(fontmgr)
  , width(0)
  , isMain(children == nil)
{
  if (children) {
    // todo: ugly copy
    topMenuItems = *children;
  } else {
    MenuItem* f;
    f = &topMenuItems.emplace_back("File", 12900);
    f->children.emplace_back("Load Side...", 12901);
    f->children.emplace_back("Duplicate Side...", 12902);
    f->children.emplace_back("Reload Side...", 12903);
    f->children.emplace_back("");
    f->children.emplace_back("Remove Side", 12905);
    f->children.emplace_back("Remove All Sides", 12906);
    f->children.emplace_back("");
    f->children.emplace_back("Exit", 12910);

    f = &topMenuItems.emplace_back("Window", 13000);
    f->children.emplace_back("&Roster", 13001);
    f->children.emplace_back("&Minimap", 13003);
    f->children.emplace_back("&Statistics", 13004);
    f->children.emplace_back("T&ypes", 13005);
    f->children.emplace_back("&Debugger", 13006);
    f->children.emplace_back("&Tournament", 13007);
    f->children.emplace_back("S&ide Debugger", 13008);

    f = &topMenuItems.emplace_back("View", 13100);
    f->children.emplace_back("Sound", 13101); // GRAYED
    f->children.emplace_back("");
    f->children.emplace_back("Show &Sensors", 13103);
    f->children.emplace_back("Show &Decorations", 13104); // CHECKED
    f->children.emplace_back("Show &Meters", 13105); // CHECKED
    f->children.emplace_back("");
    f->children.emplace_back("Report &Errors", 13113); // CHECKED
    f->children.emplace_back("Report &Prints", 13114);
    f->children.emplace_back("");
    f->children.emplace_back("Refollow", 13116);
    f->children.emplace_back("Follow Random", 13117);
    f->children.emplace_back("Follow Near", 13118);
    f->children.emplace_back("&Autofollow", 13119);
    f->children.emplace_back("");
    f->children.emplace_back("&Graph All Rounds", 13121);

    f = &topMenuItems.emplace_back("Minimap");
    f->children.emplace_back("Minimap Robots", 13107); // CHECKED
    f->children.emplace_back("Minimap Food", 13108); // CHECKED
    f->children.emplace_back("Minimap Sensors", 13109);
    f->children.emplace_back("Minimap Decorations", 13110);
    f->children.emplace_back("Minimap Trails", 13111);

    f = &topMenuItems.emplace_back("Simulation", 13200);
    f->children.emplace_back("&Run", 13201);
    f->children.emplace_back("Single &Frame", 13202);
    f->children.emplace_back("Step", 13203); // GRAYED
    f->children.emplace_back("&Pause", 13204);
    f->children.emplace_back("");
    f->children.emplace_back("Slowest (2 fps)", 13206);
    f->children.emplace_back("Slow (&10 fps)", 13207);
    f->children.emplace_back("Normal (&30 fps)", 13208); // CHECKED
    f->children.emplace_back("Fast (&60 fps)", 13209);
    f->children.emplace_back("Faster (300 fps)", 13210);
    f->children.emplace_back("&Unlimited", 13211);

    f = &topMenuItems.emplace_back("Rounds");
    f->children.emplace_back("&New Round", 13213);
    f->children.emplace_back("R&estart", 13214);
    f->children.emplace_back("&Seed", 13215);
    f->children.emplace_back("Resee&d", 13216);
    f->children.emplace_back("");
    f->children.emplace_back("Rules", 13218); // GRAYED
    f->children.emplace_back("&Tournament", 13219);
    f->children.emplace_back("Sa&ve Scores", 13220);
    f->children.emplace_back("Reset Sc&ores", 13221);
    f->children.emplace_back("Start/Stop &Brain", 13223);

    f = &topMenuItems.emplace_back("Tools", 13300);
    f->children.emplace_back("Scroll", 13301);
    f->children.emplace_back("");
    f->children.emplace_back("Add Manna", 13303);
    f->children.emplace_back("Add Robot", 13304);
    f->children.emplace_back("Add Seed", 13305);
    f->children.emplace_back("");
    f->children.emplace_back("Move", 13307);
    f->children.emplace_back("Pull", 13308);
    f->children.emplace_back("Smite", 13309);
    f->children.emplace_back("Blasts", 13310);
    f->children.emplace_back("Erase", 13311);
    f->children.emplace_back("Erase Area", 13312);

    f = &topMenuItems.emplace_back("Help", 12800);
    f->children.emplace_back("&About Grobots", 12801);
  }
}


GBMenuView::~GBMenuView() {}

void GBMenuView::Draw() {
	DrawBackground(GBColor::black);

  width = 10;
  for(auto &item : topMenuItems) {
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
  for(auto it = topMenuItems.begin(); it != topMenuItems.end(); it++) {
    auto nx = std::next(it);
    if (x > it->x && (nx == topMenuItems.end() || x < nx->x)) {
      if (!it->children.empty()) {
        app.OpenView(std::make_shared<GBMenuView>(app, fontmgr, &it->children), -1, 0);
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
#endif
