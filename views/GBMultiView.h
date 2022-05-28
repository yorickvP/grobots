// GBMultiView.h
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#ifndef GBMultiView_h
#define GBMultiView_h

#include "GBView.h"
#include <list>
#include <optional>
#include <memory>
class GBCompositedWindow;
class GBMultiView : public GBWrapperView {
 private:
  std::list<std::shared_ptr<GBCompositedWindow>> children;
  std::optional<std::weak_ptr<GBCompositedWindow>> dragging;
  std::shared_ptr<GBCompositedWindow> WindowFromXY(short x, short y);
  bool changed;
 public:
	GBMultiView(std::shared_ptr<GBView> bg);
	~GBMultiView();
  virtual void Draw_(bool) override;
  void Add(std::shared_ptr<GBView> v, short x, short y);
	virtual void AcceptClick(short x, short y, int clicksBefore) override;
	virtual void AcceptDrag(short x, short y) override;
	virtual void AcceptUnclick(short x, short y, int clicksBefore) override;
  void RightClick(short x, short y);
  virtual bool NeedsRedraw(bool running) const override;
};

#endif
