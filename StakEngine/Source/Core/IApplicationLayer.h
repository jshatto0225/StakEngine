#pragma once

#include <vector>

#include "Events.h"

class IApplicationLayer {
public:
  virtual ~IApplicationLayer() = default;

  virtual void OnAttach() {}
  virtual void OnDetach() {}
  virtual void Update() {}
  virtual void FixedUpdate(FFloat deltaTime) {}
  virtual void OnWindowResize(const FWindowResizeEvent &Event) {}
  virtual void OnImGuiRender() {}
};

class FLayerStack {
public:
  ~FLayerStack() {
    Clear();
  }

  inline void Clear() {
    while (!mLayers.empty()) {
      delete Pop();
    }
  }

  inline void Push(IApplicationLayer *layer) {
    mLayers.push_back(layer);
    layer->OnAttach();
  }

  inline IApplicationLayer *Pop() {
    IApplicationLayer *layer = *mLayers.rbegin();
    mLayers.pop_back();
    layer->OnDetach();
    return layer;
  }

  inline void Remove(IApplicationLayer *layer) {
    std::vector<IApplicationLayer *>::iterator it = std::find(mLayers.begin(), mLayers.end(), layer);
    if (it != mLayers.end()) {
      layer->OnDetach();
      mLayers.erase(it);
      delete layer;
    }
  }

  inline std::vector<IApplicationLayer *>::iterator begin() { return mLayers.begin(); }
  inline std::vector<IApplicationLayer *>::iterator end() { return mLayers.end(); }

private:
  std::vector<IApplicationLayer *> mLayers;
};
