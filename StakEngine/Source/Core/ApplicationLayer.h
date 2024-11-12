#pragma once

#include <vector>

#include "Event.h"

namespace Stak {

class IApplicationLayer {
public:
  virtual ~IApplicationLayer() = default;

  virtual void onAttach() {}
  virtual void onDetach() {}
  virtual void update() {}
  virtual void fixedUpdate(f32 deltaTime) {}
  virtual void onEvent(const IEvent &event) {}
  virtual void onImGuiRender() {}
};

class LayerStack {
public:
  ~LayerStack() {
    clear();
  }

  inline void clear() {
    for (IApplicationLayer *layer : mLayers) {
      layer->onDetach();
      delete layer;
    }
  }

  inline void push(IApplicationLayer *layer) {
    mLayers.push_back(layer);
  }

  inline IApplicationLayer *pop() {
    IApplicationLayer *layer = *mLayers.rbegin();
    mLayers.pop_back();
    return layer;
  }

  inline void remove(IApplicationLayer *layer) {
    std::vector<IApplicationLayer *>::iterator it = std::find(mLayers.begin(), mLayers.end(), layer);
    if (it != mLayers.end()) {
      layer->onDetach();
      mLayers.erase(it);
    }
  }

  inline std::vector<IApplicationLayer *>::iterator begin() { return mLayers.begin(); }
  inline std::vector<IApplicationLayer *>::iterator end() { return mLayers.end(); }

private:
  std::vector<IApplicationLayer *> mLayers;
};

} // namespace Stak
