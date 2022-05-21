// GBMenu.h

#ifdef WITH_SDL

#ifndef GBMenu_h
#define GBMenu_h

#include "GBView.h"
#include "GBWorld.h"

class GBSDLApplication;
class GBMenuView : public GBView {
	GBWorld& world;
	GBSDLApplication& app;
public:
	GBMenuView(GBWorld& world, GBSDLApplication& app);
	~GBMenuView();

	void Draw();
	
	short PreferredWidth() const;
	short PreferredHeight() const;
	
	const string Name() const;
	
	void AcceptClick(short x, short y, int /*clicks*/);
};

#endif
#endif
