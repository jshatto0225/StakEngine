#pragma once

#include <vector>

#include "Event.h"

namespace Stak {

class ApplicationLayer {
public:
  virtual ~ApplicationLayer() = default;

  virtual void onAttach() {}
  virtual void onDetach() {}
  virtual void update() {}
  virtual void fixedUpdate(f32 deltaTime) {}
  virtual void onEvent(const Event &event) {}
  virtual void onImGuiRender() {}
};

class LayerStack {
public:
  ~LayerStack() {
    for (ApplicationLayer *layer : mLayers) {
      layer->onDetach();
      delete layer;
    }
  }

  inline void push(ApplicationLayer *layer) {
    mLayers.push_back(layer);
  }

  inline ApplicationLayer *pop() {
    ApplicationLayer *layer = *mLayers.rbegin();
    mLayers.pop_back();
    return layer;
  }

  inline void remove(ApplicationLayer *layer) {
    std::vector<ApplicationLayer *>::iterator it = std::find(mLayers.begin(), mLayers.end(), layer);
    if (it != mLayers.end()) {
      layer->onDetach();
      mLayers.erase(it);
    }
  }

  inline std::vector<ApplicationLayer *>::iterator begin() { return mLayers.begin(); }
  inline std::vector<ApplicationLayer *>::iterator end() { return mLayers.end(); }

private:
  std::vector<ApplicationLayer *> mLayers;
};

} // namespace Stak
