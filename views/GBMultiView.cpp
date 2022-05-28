// GBMultiView.cpp
// Grobots (c) 2002-2004 Devon and Warren Schudy
// Distributed under the GNU General Public License.

#include "GBView.h"
#include "GBErrors.h"
#include "GBColor.h"
#include "GBStringUtilities.h"
#include "GBTypes.h"
#include "GBMultiView.h"
#include <memory>
#include <optional>

const short kTitleBarHeight = 16;
const short kFrameSize = 1;

class GBCompositedWindow {
  std::shared_ptr<GBView> v;
  GBGraphics& parent;
  std::unique_ptr<GBBitmap> texture;
  short lastX, lastY;
public:
  GBCompositedWindow(std::shared_ptr<GBView> v, GBGraphics& parent, short x, short y) :
    v(v), parent(parent), texture(std::make_unique<GBBitmap>(v->PreferredWidth() + kFrameSize * 2, v->PreferredHeight() + kTitleBarHeight + 2 * kFrameSize, parent)), lastX(-1), lastY(-1) {
    texture->SetPosition(x, y);
    GBRect bounds = GBRect(kFrameSize,
                           kTitleBarHeight + kFrameSize,
                           v->PreferredWidth() + kFrameSize,
                           v->PreferredHeight() + kTitleBarHeight + kFrameSize);
    v->SetBounds(bounds);
    DrawFrame();
    Draw(true, false);
  };
  GBCompositedWindow(const GBCompositedWindow&) = delete;
  ~GBCompositedWindow() {};
  bool Matches(const string & name) const {
    return v->Name().compare(name) == 0;
  };
  void Resize() {
    GBRect oldBounds = texture->Bounds();
    const short newHeight = v->PreferredHeight();
    const short newWidth = v->PreferredWidth();
    texture = std::make_unique<GBBitmap>(newWidth + kFrameSize * 2, newHeight + kTitleBarHeight + 2 * kFrameSize, parent);
    texture->SetPosition(oldBounds.left, oldBounds.top);
    v->SetSize(newWidth, newHeight);
    DrawFrame();
    Draw(true, false);
  }
  void DrawFrame() {
    GBGraphicsWrapper g = texture->Graphics();
    short width = texture->Bounds().Width();
    short height = texture->Bounds().Height();
    // frame
    g->DrawOpenRect(GBRect(0, 0, width, height), GBColor::white);
    GBRect titlebar = GBRect(0, 0, width, kTitleBarHeight+1);
    // titlebar bg // todo: alpha?
    g->DrawSolidRect(titlebar, GBColor::black);
    g->DrawOpenRect(titlebar, GBColor::white);
    g->DrawStringCentered(v->Name(), width / 2, kTitleBarHeight + 1, 14, GBColor::white, true);
  };
  void Draw(bool force, bool running) {
    if (v->NeedsResize()) return Resize();
    // todo: clip
    if (force || v->NeedsRedraw(running)) {
      GBGraphicsWrapper g = texture->Graphics();
      texture->SetClip(&v->Bounds());
      v->DoDraw(**g, running);
      texture->SetClip(nil);
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
    if (y - dst.top < v->Bounds().top) {
      lastX = x;
      lastY = y;
      return;
      // todo: move by drag on unused client space
      // possibly by returning bool from DoClick
    }
    v->DoClick(x - dst.left, y - dst.top, clicksBefore);
  };
  void DoUnclick(short x, short y, int clicksBefore) {
    const GBRect& dst = texture->Bounds();
    v->DoUnclick(x - dst.left, y - dst.top, clicksBefore);
    lastX = -1;
    lastY = -1;
  };
  void DoDrag(short x, short y) {
    const GBRect& dst = texture->Bounds();
    if (y - dst.top > v->Bounds().top) {
      v->DoDrag(x - dst.left, y - dst.top);
    }
    if (lastX != -1 && lastY != -1) {
      texture->SetPosition(dst.left + (x - lastX), dst.top + (y - lastY));
      lastX = x;
      lastY = y;
    }
  };
  bool NeedsRedraw(bool running) const {
    return v->NeedsRedraw(running);
  }
  bool NeedsResize() const {
    return v->NeedsResize();
  }
};

GBMultiView::GBMultiView(std::shared_ptr<GBView> bg)
	: GBWrapperView(bg), children(), dragging(), changed(true)
{}

GBMultiView::~GBMultiView() {}

void GBMultiView::Draw_(bool running) {
  content->DoDraw(Graphics(), running);
  for (auto const & childView : children) {
    childView->Draw(false, running);
    childView->Blit(Graphics());
  }
  if (changed) changed = false;
}

bool GBMultiView::NeedsRedraw(bool running) const {
  if (changed) return true;
  if (content->NeedsRedraw(running)) return true;
  for (auto const & childView : children) {
    if (childView->NeedsRedraw(running)) return true;
    if (childView->NeedsResize()) return true;
  }
  return false;
}

std::shared_ptr<GBCompositedWindow> GBMultiView::WindowFromXY(short x, short y) {
  for (auto rit = children.rbegin(); rit != children.rend(); ++rit)
    if ((*rit)->HasPoint(x, y)) return *rit;
  return {};
}
void GBMultiView::AcceptClick(short x, short y, int clicksBefore) {
  if (auto childView = WindowFromXY(x, y)) {
    dragging = childView;
    (*childView).DoClick(x, y, clicksBefore);
    return;
  }
  content->DoClick(x, y, clicksBefore);
}
void GBMultiView::AcceptUnclick(short x, short y, int clicksBefore) {
  if (dragging && dragging->expired()) {
    dragging = {};
    return;
  }
  dragging = {};
  if (auto childView = WindowFromXY(x, y)) {
    (*childView).DoUnclick(x, y, clicksBefore);
    return;
  }
  content->DoUnclick(x, y, clicksBefore);
}
void GBMultiView::AcceptDrag(short x, short y) {
  // todo: was dragging, but target disappeared
  if (auto dr = dragging) {
    if (auto d = dr->lock()) d->DoDrag(x, y);
  } else content->DoDrag(x, y);
  changed = true;
}

void GBMultiView::Add(std::shared_ptr<GBView> v, short x, short y) {
  changed = true;
  // hack to prevent duplicate windows
  for (const auto &childView : children) {
    if (childView->Matches(v->Name())) {
      return;
    }
  }
  children.push_back(std::make_shared<GBCompositedWindow>(v, Graphics(), x, y));
}

void GBMultiView::RightClick(short x, short y) {
  if (auto childView = WindowFromXY(x, y)) {
    // prevent closing menu
    if (childView == children.front()) return;
    children.remove(childView);
    changed = true;
  }
}

// const string GBMultiView::Name() const {
// 	return "Grobots";
// }

