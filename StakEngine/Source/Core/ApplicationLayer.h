#pragma once

#include <vector>

#include "Event.h"

namespace Stak {

class ApplicationLayer {
public:
  virtual ~ApplicationLayer() = default;

  virtual void onAttach() {};
  virtual void onDetach() {};
  virtual void update() {};
  virtual void fixedUpdate(f32 deltaTime) {};
  virtual void onEvent(const Event &event) {};
};

class LayerStack {
public:
  ~LayerStack() {
    for (ApplicationLayer *layer : m_Layers) {
      layer->onDetach();
      delete layer;
    }
  }

  inline void push(ApplicationLayer *layer) {
    m_Layers.push_back(layer);
  }

  inline ApplicationLayer *pop() {
    ApplicationLayer *layer = *m_Layers.rbegin();
    m_Layers.pop_back();
    return layer;
  }

  inline void remove(ApplicationLayer *layer) {
    std::vector<ApplicationLayer *>::iterator it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if (it != m_Layers.end()) {
      layer->onDetach();
      m_Layers.erase(it);
    }
  }

  inline std::vector<ApplicationLayer *>::iterator begin() { return m_Layers.begin(); }
  inline std::vector<ApplicationLayer *>::iterator end() { return m_Layers.end(); }

private:
  std::vector<ApplicationLayer *> m_Layers;
};

} // namespace Stak
