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

const short kItemHeight = 20;
const short kSeparatorHeight = 8;
const short kCheckColumnWidth = 16;
const short kShortcutPadding = 20;
const short kMenuBarHeight = 16;

static void SetShortcut(std::shared_ptr<std::list<MenuItem>> items, unsigned int id, char key, bool ctrl = true) {
  MenuItem* item = GBMenuView::FindItemInList(items, id);
  if (item) {
    item->key = key;
    item->ctrlKey = ctrl;
  }
}

GBMenuView::GBMenuView(GBSDLApplication& app, GBFontManager& fontmgr, std::shared_ptr<std::list<MenuItem>> children)
	: GBView()
	, app(app)
  , fontmgr(fontmgr)
  , width(0)
  , height(0)
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
    #ifndef __EMSCRIPTEN__
    f->children->emplace_back("Reload Side", miReloadSide);
    #endif
    f->children->emplace_back("");
    f->children->emplace_back("Remove Side", miRemoveSide);
    f->children->emplace_back("Remove All Sides", miRemoveAllSides);
    #ifndef __EMSCRIPTEN__
    f->children->emplace_back("");
    f->children->emplace_back("Exit", miQuit);
    #endif

    f = &topMenuItems->emplace_back("Window");
    f->children->emplace_back("&Roster", miRosterView);
    f->children->emplace_back("&Minimap", miMinimapView);
    f->children->emplace_back("&Statistics", miScoresView);
    f->children->emplace_back("T&ypes", miTypesView);
    f->children->emplace_back("&Debugger", miDebuggerView);
    f->children->emplace_back("&Tournament", miTournamentView);
    f->children->emplace_back("Shared Memory", miSideDebuggerView);

    f = &topMenuItems->emplace_back("View");
    f->children->emplace_back("Sound", miSound);
    f->children->emplace_back("");
    f->children->emplace_back("Zoom In", miZoomIn);
    f->children->emplace_back("Zoom Out", miZoomOut);
    f->children->emplace_back("Zoom Standard", miZoomStandard);
    f->children->emplace_back("");
    f->children->emplace_back("Show &Sensors", miShowSensors);
    f->children->emplace_back("Show &Decorations", miShowDecorations);
    f->children->emplace_back("Show &Meters", miShowMeters);
    f->children->emplace_back("Minimap Trails", miMinimapTrails);
    f->children->emplace_back("");
    f->children->emplace_back("Report &Errors", miReportErrors);
    f->children->emplace_back("Report &Prints", miReportPrints);
    f->children->emplace_back("");
    f->children->emplace_back("Refollow", miRefollow);
    f->children->emplace_back("Follow Random", miFollowRandom);
    f->children->emplace_back("Follow Near", miRandomNear);
    f->children->emplace_back("&Autofollow", miAutofollow);
    f->children->emplace_back("");
    f->children->emplace_back("Next Page", miNextPageMemory);
    f->children->emplace_back("Previous Page", miPreviousPageMemory);
    f->children->emplace_back("First Page", miFirstPageMemory);

    f = &topMenuItems->emplace_back("Simulation");
    f->children->emplace_back("&Run", miRun);
    f->children->emplace_back("Single &Frame", miSingleFrame);
    f->children->emplace_back("Step", miStep);
    f->children->emplace_back("&Pause", miPause);
    f->children->emplace_back("Start/Stop &Brain", miStartStopBrain);
    f->children->emplace_back("");
    f->children->emplace_back("Slowest (2 fps)", miSlowerSpeed);
    f->children->emplace_back("Slow (&10 fps)", miSlowSpeed);
    f->children->emplace_back("Normal (&30 fps)", miNormalSpeed);
    f->children->emplace_back("Fast (&60 fps)", miFastSpeed);
    f->children->emplace_back("Faster (300 fps)", miFasterSpeed);
    f->children->emplace_back("&Unlimited", miUnlimitedSpeed);

    f = &topMenuItems->emplace_back("Rounds");
    f->children->emplace_back("&New Round", miNewRound);
    f->children->emplace_back("Clear World", miRestart);
    f->children->emplace_back("Add Seeds", miSeed);
    f->children->emplace_back("Reseed Dead Sides", miReseed);
    f->children->emplace_back("");
    f->children->emplace_back("Rules", miRules);
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

    // Assign keyboard shortcuts
    SetShortcut(topMenuItems, miLoadSide, 'o');
    SetShortcut(topMenuItems, miReloadSide, 'l');
    SetShortcut(topMenuItems, miRemoveSide, 'k');
    SetShortcut(topMenuItems, miQuit, 'q');
    // Window
    SetShortcut(topMenuItems, miMinimapView, 'm');
    SetShortcut(topMenuItems, miScoresView, 's');
    SetShortcut(topMenuItems, miTypesView, 'y');
    SetShortcut(topMenuItems, miDebuggerView, 'd');
    SetShortcut(topMenuItems, miTournamentView, 'e');
    // View
    SetShortcut(topMenuItems, miZoomIn, '=', false);
    SetShortcut(topMenuItems, miZoomOut, '-', false);
    SetShortcut(topMenuItems, miZoomStandard, '0', false);
    SetShortcut(topMenuItems, miAutofollow, 'a');
    SetShortcut(topMenuItems, miMinimapTrails, 't', false);
    SetShortcut(topMenuItems, miRefollow, '`', false);
    SetShortcut(topMenuItems, miFollowRandom, '\r', false);
    SetShortcut(topMenuItems, miRandomNear, '\t', false);
    // Simulation
    SetShortcut(topMenuItems, miRun, 'r');
    SetShortcut(topMenuItems, miSingleFrame, 'f');
    SetShortcut(topMenuItems, miPause, 'p');
    SetShortcut(topMenuItems, miNewRound, 'n');
    SetShortcut(topMenuItems, miTournament, 't');
    SetShortcut(topMenuItems, miUnlimitedSpeed, 'u');
  }
}


GBMenuView::~GBMenuView() {}

std::string GBMenuView::StripAmpersand(const std::string& name) {
  std::string result;
  result.reserve(name.size());
  for (size_t i = 0; i < name.size(); ++i) {
    if (name[i] == '&') continue;
    result += name[i];
  }
  return result;
}

std::string GBMenuView::ShortcutText(const MenuItem& item) {
  if (item.key == 0) return "";
  std::string text;
  if (item.ctrlKey) {
#if __APPLE__
    text = "Cmd+";
#else
    text = "Ctrl+";
#endif
  }
  if (item.key >= 'a' && item.key <= 'z') {
    text += (char)(item.key - 32); // uppercase
  } else if (item.key == '\r') {
    text += "Enter";
  } else if (item.key == '\t') {
    text += "Tab";
  } else if (item.key == '`') {
    text += "`";
  } else {
    text += item.key;
  }
  return text;
}

MenuItem* GBMenuView::FindItemInList(std::shared_ptr<std::list<MenuItem>> items, unsigned int id) {
  if (!items) return nullptr;
  for (auto& item : *items) {
    if (item.id == id) return &item;
    MenuItem* found = FindItemInList(item.children, id);
    if (found) return found;
  }
  return nullptr;
}

void GBMenuView::EnableItem(unsigned int id, bool enabled) {
  MenuItem* item = FindItemInList(topMenuItems, id);
  if (item) item->enabled = enabled;
}

void GBMenuView::CheckItem(unsigned int id, bool checked) {
  MenuItem* item = FindItemInList(topMenuItems, id);
  if (item) item->checked = checked;
}

int GBMenuView::FindShortcut(SDL_Keycode key, SDL_Keymod mod) const {
  bool ctrl = (mod & SDL_KMOD_CTRL) != 0;
  bool gui = (mod & SDL_KMOD_GUI) != 0;
  bool cmdOrCtrl = ctrl || gui;
  // Convert SDL keycode to our char representation
  char ch = 0;
  if (key >= SDLK_A && key <= SDLK_Z) {
    ch = 'a' + (key - SDLK_A);
  } else if (key == SDLK_RETURN) {
    ch = '\r';
  } else if (key == SDLK_TAB) {
    ch = '\t';
  } else if (key == SDLK_GRAVE) {
    ch = '`';
  } else if (key == SDLK_EQUALS) {
    ch = '=';
  } else if (key == SDLK_MINUS) {
    ch = '-';
  } else if (key == SDLK_0) {
    ch = '0';
  } else if (key >= SDLK_1 && key <= SDLK_9) {
    ch = '0' + (key - SDLK_0);
  }
  if (ch == 0) return 0;

  // Search all items recursively
  std::function<int(const std::list<MenuItem>&)> search = [&](const std::list<MenuItem>& items) -> int {
    for (const auto& item : items) {
      if (item.key == ch && item.enabled) {
        if (item.ctrlKey && cmdOrCtrl) return item.id;
        if (!item.ctrlKey && !cmdOrCtrl) return item.id;
      }
      int found = search(*item.children);
      if (found) return found;
    }
    return 0;
  };
  return search(*topMenuItems);
}

void GBMenuView::Draw() {
  DrawBackground(GBColor::black);

  if (isMain) {
    // Horizontal menu bar
    width = 10;
    for (auto &item : *topMenuItems) {
      item.x = width;
      if (item.name.length() == 0) {
        DrawLine(width, 0, width, kMenuBarHeight, GBColor::white, 1);
        width += 11;
        continue;
      }
      std::string display = StripAmpersand(item.name);
      DrawStringLeft(display, width, kMenuBarHeight, 14, GBColor::white);
      width += Graphics().MeasureText(display, 14, GBColor::white).Width() + 10;
    }
    height = kMenuBarHeight;
  } else {
    // Vertical submenu
    // First pass: compute width
    short maxNameWidth = 0;
    short maxShortcutWidth = 0;
    for (auto &item : *topMenuItems) {
      if (item.name.empty()) continue;
      std::string display = StripAmpersand(item.name);
      short nameW = Graphics().MeasureText(display, 14, GBColor::white).Width();
      if (nameW > maxNameWidth) maxNameWidth = nameW;
      std::string sc = ShortcutText(item);
      if (!sc.empty()) {
        short scW = Graphics().MeasureText(sc, 12, GBColor::gray).Width();
        if (scW > maxShortcutWidth) maxShortcutWidth = scW;
      }
    }
    width = kCheckColumnWidth + maxNameWidth + 10;
    if (maxShortcutWidth > 0) width += kShortcutPadding + maxShortcutWidth;
    width += 8; // right padding

    // Second pass: draw items
    short y = 0;
    for (auto &item : *topMenuItems) {
      if (item.name.empty()) {
        // Separator
        DrawLine(0, y + kSeparatorHeight / 2, width, y + kSeparatorHeight / 2, GBColor::gray, 1);
        y += kSeparatorHeight;
        continue;
      }
      item.x = y; // reuse x field to store y position for vertical menus

      GBColor textColor = item.enabled ? GBColor::white : GBColor(0.4f, 0.4f, 0.4f);

      // Checkmark
      if (item.checked) {
        DrawStringLeft("\xE2\x80\xA2", 4, y + kItemHeight, 14, textColor); // bullet
      }

      // Item name
      std::string display = StripAmpersand(item.name);
      DrawStringLeft(display, kCheckColumnWidth, y + kItemHeight, 14, textColor);

      // Shortcut hint
      std::string sc = ShortcutText(item);
      if (!sc.empty()) {
        short scW = Graphics().MeasureText(sc, 12, GBColor::gray).Width();
        DrawStringLeft(sc, width - scW - 8, y + kItemHeight, 12,
                       item.enabled ? GBColor::gray : GBColor(0.3f, 0.3f, 0.3f));
      }

      y += kItemHeight;
    }
    height = y;
  }
}

short GBMenuView::PreferredWidth() const {
  if (width == 0) return 260;
  return width;
}

short GBMenuView::MinimumWidth() const {
  if (width == 0) return 260;
  return width;
}

short GBMenuView::PreferredHeight() const {
  if (!isMain && height > 0) return height;
  return kMenuBarHeight;
}

const string GBMenuView::Name() const {
	return "";
}

void GBMenuView::AcceptClick(short x, short y, int /*clicks*/) {
  if (!width) return; // need draw first

  if (isMain) {
    // Horizontal menu bar
    if (y > kMenuBarHeight) return;
    for (auto it = topMenuItems->begin(); it != topMenuItems->end(); it++) {
      auto nx = std::next(it);
      if (x > (short)it->x && (nx == topMenuItems->end() || x < (short)nx->x)) {
        if (!it->children->empty()) {
          // If a submenu is already open, close it
          if (auto existing = openSubmenu.lock()) {
            app.CloseView(*existing);
            openSubmenu = {};
            // If clicking the same menu item, just toggle closed
            if (existing->topMenuItems == it->children) return;
          }
          app.AdjustMenus();
          auto sub = std::make_shared<GBMenuView>(app, fontmgr, it->children);
          openSubmenu = sub;
          app.OpenView(sub, it->x, kMenuBarHeight);
        } else {
          if (it->id != 0) {
            app.HandleMenuSelection(it->id);
          }
        }
        return;
      }
    }
  } else {
    // Vertical submenu - hit test by y coordinate
    short itemY = 0;
    for (auto &item : *topMenuItems) {
      if (item.name.empty()) {
        itemY += kSeparatorHeight;
        continue;
      }
      if (y >= itemY && y < itemY + kItemHeight) {
        if (!item.enabled) return; // grayed out
        if (!item.children->empty()) {
          app.AdjustMenus();
          app.OpenView(std::make_shared<GBMenuView>(app, fontmgr, item.children), width, itemY);
        } else if (item.id != 0) {
          app.HandleMenuSelection(item.id);
          app.CloseView(*this);
        }
        return;
      }
      itemY += kItemHeight;
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
