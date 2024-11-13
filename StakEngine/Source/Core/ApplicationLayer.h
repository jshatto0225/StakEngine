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
    while (mLayers.size() > 0) {
      delete pop();
    }
  }

  inline void push(IApplicationLayer *layer) {
    mLayers.push_back(layer);
    layer->onAttach();
  }

  inline IApplicationLayer *pop() {
    IApplicationLayer *layer = *mLayers.rbegin();
    mLayers.pop_back();
    layer->onDetach();
    return layer;
  }

  inline void remove(IApplicationLayer *layer) {
    std::vector<IApplicationLayer *>::iterator it = std::find(mLayers.begin(), mLayers.end(), layer);
    if (it != mLayers.end()) {
      layer->onDetach();
      mLayers.erase(it);
      delete layer;
    }
  }

  inline std::vector<IApplicationLayer *>::iterator begin() { return mLayers.begin(); }
  inline std::vector<IApplicationLayer *>::iterator end() { return mLayers.end(); }

private:
  std::vector<IApplicationLayer *> mLayers;
};

} // namespace Stak
