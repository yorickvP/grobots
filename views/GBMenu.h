// GBMenu.h

#ifdef WITH_SDL

#ifndef GBMenu_h
#define GBMenu_h

#include "GBView.h"
#include "GBWorld.h"
#include <list>
#include <string>

struct MenuItem {
  MenuItem(std::string name, unsigned int id=0) : name(name), id(id) {};
  std::string name;
  unsigned int id = 0;
  std::list<MenuItem> children;
  unsigned int x = 0;
};
class GBSDLApplication;
class GBMenuView : public GBView {
	GBSDLApplication& app;
  GBFontManager& fontmgr;
  unsigned int width;
  std::list<MenuItem> topMenuItems;
  bool isMain;
public:
	GBMenuView(GBSDLApplication& app, GBFontManager&, std::list<MenuItem>* menuItems = nil);
	~GBMenuView();

	void Draw();
	
	short PreferredWidth() const;
	short PreferredHeight() const;
  short MinimumWidth() const override;
  bool GetFrontClicks() const override;
	
	const string Name() const;
	
	void AcceptClick(short x, short y, int /*clicks*/);
  void SetFocus(bool) override;
};

#endif
#endif
