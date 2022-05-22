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
 public:
	GBMultiView(GBView* const bg);
	~GBMultiView();
  void Draw();
  void Add(GBView& v);
	virtual void AcceptClick(short x, short y, int clicksBefore) override;
	virtual void AcceptDrag(short x, short y) override;
	virtual void AcceptUnclick(short x, short y, int clicksBefore) override;
  virtual bool InstantChanges() const override;
  virtual bool DelayedChanges() const override;
  void RightClick(short x, short y);
};

#endif
