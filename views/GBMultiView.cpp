// GBMultiView.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBView.h"
#include "GBErrors.h"
#include "GBColor.h"
#include "GBStringUtilities.h"
#include "GBTypes.h"
#include "GBMultiView.h"
#include <ranges>
#include <memory>

const short kTitleBarHeight = 16;
const short kFrameSize = 1;

class GBCompositedWindow {
  GBView& v;
  GBGraphics& parent;
  std::unique_ptr<GBBitmap> texture;
  short lastX, lastY;
public:
  GBCompositedWindow(GBView& v, GBGraphics& parent) :
    v(v), parent(parent), texture(new GBBitmap(v.PreferredWidth() + kFrameSize * 2, v.PreferredHeight() + kTitleBarHeight + 2 * kFrameSize, parent)), lastX(-1), lastY(-1) {
    GBRect bounds = GBRect(kFrameSize,
                           kTitleBarHeight + kFrameSize,
                           v.PreferredWidth() + kFrameSize,
                           v.PreferredHeight() + kTitleBarHeight + kFrameSize);
    v.SetBounds(bounds);
    Draw(true);
  };
  GBCompositedWindow(const GBCompositedWindow&) = delete;
  ~GBCompositedWindow() {
    delete &v;
  };
  void Resize() {
    GBRect oldBounds = texture->Bounds();
    const short newHeight = v.PreferredHeight();
    const short newWidth = v.PreferredWidth();
    texture.reset(new GBBitmap(newWidth + kFrameSize * 2, newHeight + kTitleBarHeight + 2 * kFrameSize, parent));
    texture->SetPosition(oldBounds.left, oldBounds.top);
    // GBRect innerBounds = GBRect(kFrameSize,
    //                        kTitleBarHeight + kFrameSize,
    //                        newWidth + kFrameSize,
    //                        newHeight + kTitleBarHeight + kFrameSize);
    v.SetSize(newWidth, newHeight);
    // v.SetBounds(innerBounds);
    Draw(true);
  }
  void DrawFrame() {
    //texture->StartDrawing();
    GBGraphics& g = texture->Graphics();
    short width = texture->Bounds().Width();
    //short height = texture->Bounds().Height();
    // frame
    g.DrawOpenRect(texture->Bounds(), GBColor::white);
    GBRect titlebar = GBRect(0, 0, width, kTitleBarHeight+1);
    // titlebar bg // todo: alpha?
    g.DrawSolidRect(titlebar, GBColor::black);
    g.DrawOpenRect(titlebar, GBColor::white);
    g.DrawStringCentered(v.Name(), width / 2, kTitleBarHeight + 1, 14, GBColor::white, true);
  };
  void Draw(bool force) {
    if (v.NeedsResize()) return Resize();
    // todo: clip
    if (force || v.NeedsRedraw(false)) {
      v.SetGraphics(&texture->Graphics());
      texture->StartDrawing();
      DrawFrame();
      texture->SetClip(&v.Bounds());
      v.DoDraw();
      texture->StopDrawing();
    }
  };
  void Blit(GBGraphics& dest) {
    const GBRect& dst = texture->Bounds();
    GBRect src = GBRect(0, 0, dst.Width(), dst.Height());
    dest.Blit(*texture, src, dst, 200);
  };
  bool HasPoint(short x, short y) const {
    return texture->Bounds().HasPoint(x, y);
  };
  void DoClick(short x, short y, int clicksBefore) {
    const GBRect& dst = texture->Bounds();
    lastX = x;
    lastY = y;
    if (y - dst.top < v.Bounds().top) return;
    v.DoClick(x - dst.left, y - dst.top, clicksBefore);
  };
  void DoUnclick(short x, short y, int clicksBefore) {
    const GBRect& dst = texture->Bounds();
    v.DoUnclick(x - dst.left, y - dst.top, clicksBefore);
    lastX = -1;
    lastY = -1;
  };
  void DoDrag(short x, short y) {
    const GBRect& dst = texture->Bounds();
    if (y - dst.top > v.Bounds().top) {
      v.DoDrag(x - dst.left, y - dst.top);
    }
    texture->SetPosition(dst.left + (x - lastX), dst.top + (y - lastY));
    lastX = x;
    lastY = y;
  };
  bool NeedsResize() const {
    return v.NeedsResize();
  }
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

GBCompositedWindow* GBMultiView::WindowFromXY(short x, short y) {
  for (auto const & childView : children | std::views::reverse)
    if (childView->HasPoint(x, y)) return childView;
  return nil;
}
void GBMultiView::AcceptClick(short x, short y, int clicksBefore) {
  if (GBCompositedWindow* childView = WindowFromXY(x, y)) {
    dragging = childView;
    childView->DoClick(x, y, clicksBefore);
    return;
  }
  content->DoClick(x, y, clicksBefore);
}
void GBMultiView::AcceptUnclick(short x, short y, int clicksBefore) {
  dragging = nil;
  if (GBCompositedWindow* childView = WindowFromXY(x, y)) {
    childView->DoUnclick(x, y, clicksBefore);
    return;
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

void GBMultiView::RightClick(short x, short y) {
  if (GBCompositedWindow* childView = WindowFromXY(x, y)) {
    // prevent closing menu
    if (childView == children.front()) return;
    children.remove(childView);
    delete childView;
  }
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
