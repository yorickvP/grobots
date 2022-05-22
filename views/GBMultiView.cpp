// GBMultiView.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBView.h"
#include "GBErrors.h"
#include "GBColor.h"
#include "GBStringUtilities.h"
#include "GBTypes.h"
#include "GBMultiView.h"

class GBCompositedWindow {
  GBView& v;
  GBGraphics& parent;
  GBBitmap& texture;
  short lastX, lastY;
public:
  GBCompositedWindow(GBView& v, GBGraphics& parent) :
    v(v), parent(parent), texture(*new GBBitmap(v.PreferredWidth(), v.PreferredHeight(), parent)), lastX(-1), lastY(-1) {
    v.SetSize(texture.Bounds().Width(), texture.Bounds().Height());
    Draw(true);
  };
  GBCompositedWindow(const GBCompositedWindow&) = delete;
  ~GBCompositedWindow() {
    delete &v;
    delete &texture;
  };
  void Draw(bool force) {
    // todo: clip and decoration
    if (force || v.NeedsRedraw(false)) {
      v.SetGraphics(&texture.Graphics());
      texture.StartDrawing();
      v.DoDraw();
      texture.StopDrawing();
    }
  };
  void Blit(GBGraphics& dest) {
    const GBRect& dst = texture.Bounds();
    GBRect src = GBRect(0, 0, dst.Width(), dst.Height());
    dest.Blit(texture, src, dst);
  };
  bool HasPoint(short x, short y) const {
    return texture.Bounds().HasPoint(x, y);
  };
  void DoClick(short x, short y, int clicksBefore) {
    const GBRect& dst = texture.Bounds();
    v.DoClick(x - dst.left, y - dst.top, clicksBefore);
    lastX = x;
    lastY = y;
  };
  void DoUnclick(short x, short y, int clicksBefore) {
    const GBRect& dst = texture.Bounds();
    v.DoUnclick(x - dst.left, y - dst.top, clicksBefore);
    lastX = -1;
    lastY = -1;
  };
  void DoDrag(short x, short y) {
    const GBRect& dst = texture.Bounds();
    v.DoDrag(x - dst.left, y - dst.top);
    texture.SetPosition(dst.left + (x - lastX), dst.top + (y - lastY));
    lastX = x;
    lastY = y;
  };
};

GBMultiView::GBMultiView(GBView* const bg)
	: GBWrapperView(bg), children(), dragging(nil)
{}

GBMultiView::~GBMultiView() {
  for (auto const& i : children) {
    delete i;
  }
}
// todo: needsredraw
void GBMultiView::Draw() {
  content->SetGraphics(&Graphics());
  content->Draw();
  for (auto const & childView : children) {
    childView->Draw(false);
    childView->Blit(Graphics());
  }
}

void GBMultiView::AcceptClick(short x, short y, int clicksBefore) {
  for (auto const & childView : children) {
    if (childView->HasPoint(x, y)) {
      dragging = childView;
      childView->DoClick(x, y, clicksBefore);
      return;
    }
  }
  content->DoClick(x, y, clicksBefore);
}
void GBMultiView::AcceptUnclick(short x, short y, int clicksBefore) {
  dragging = nil;
  for (auto const & childView : children) {
    if (childView->HasPoint(x, y)) {
      childView->DoUnclick(x, y, clicksBefore);
      return;
    }
  }
  content->DoUnclick(x, y, clicksBefore);
}
void GBMultiView::AcceptDrag(short x, short y) {
  if (dragging) dragging->DoDrag(x, y);
  else content->DoDrag(x, y);
}

void GBMultiView::Add(GBView& v) {
  children.push_back(new GBCompositedWindow(v, Graphics()));
}

// const string GBMultiView::Name() const {
// 	return "Grobots";
// }

// TODO
bool GBMultiView::InstantChanges() const {
  return true;
}
bool GBMultiView::DelayedChanges() const {
  return true;
}
