// GBMultiView.h
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef GBMultiView_h
#define GBMultiView_h

#include "GBView.h"
#include <list>
class GBCompositedWindow;
class GBMultiView : public GBWrapperView {
 private:
  std::list<GBCompositedWindow*> children;
  GBCompositedWindow* dragging;
  GBCompositedWindow* WindowFromXY(short x, short y);
  bool changed;
 public:
	GBMultiView(GBView* const bg);
	~GBMultiView();
  virtual void Draw_(bool) override;
  void Add(GBView& v, short x, short y);
	virtual void AcceptClick(short x, short y, int clicksBefore) override;
	virtual void AcceptDrag(short x, short y) override;
	virtual void AcceptUnclick(short x, short y, int clicksBefore) override;
  void RightClick(short x, short y);
  virtual bool NeedsRedraw(bool running) const override;
};

#endif
