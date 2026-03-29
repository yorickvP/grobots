// GBMenu.h

#ifdef WITH_SDL

#ifndef GBMenu_h
#define GBMenu_h

#include "GBView.h"
#include "GBWorld.h"
#include <SDL3/SDL.h>
#include <list>
#include <string>
#include <functional>

struct MenuItem {
  MenuItem(std::string name, unsigned int id=0) : name(name), id(id),
                                                  children(std::make_shared<std::list<MenuItem>>()){};
  std::string name;
  unsigned int id = 0;
  std::shared_ptr<std::list<MenuItem>> children;
  unsigned int x = 0;
  bool enabled = true;
  bool checked = false;
  char key = 0;
  bool ctrlKey = false;  // Ctrl (or Cmd on macOS) modifier
};
class GBSDLApplication;
class GBMenuView : public GBView {
	GBSDLApplication& app;
  GBFontManager& fontmgr;
  unsigned int width;
  unsigned int height;
  std::shared_ptr<std::list<MenuItem>> topMenuItems;
  bool isMain;

  std::weak_ptr<GBMenuView> openSubmenu;

  static std::string StripAmpersand(const std::string& name);
  static std::string ShortcutText(const MenuItem& item);
public:
  static MenuItem* FindItemInList(std::shared_ptr<std::list<MenuItem>> items, unsigned int id);
	GBMenuView(GBSDLApplication& app, GBFontManager&, std::shared_ptr<std::list<MenuItem>> menuItems = nil);
	~GBMenuView();

	void Draw();

	short PreferredWidth() const;
	short PreferredHeight() const;
  short MinimumWidth() const override;
  bool GetFrontClicks() const override;

	const string Name() const;

	void AcceptClick(short x, short y, int /*clicks*/);
  void SetFocus(bool) override;

  void EnableItem(unsigned int id, bool enabled);
  void CheckItem(unsigned int id, bool checked);
  int FindShortcut(SDL_Keycode key, SDL_Keymod mod) const;
};

#endif
#endif
