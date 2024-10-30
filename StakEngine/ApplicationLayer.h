#pragma once

#include <vector>

#include "Event.h"

namespace Stak {

class ApplicationLayer {
public:
  virtual ~ApplicationLayer() = default;

  virtual void OnAttach() {};
  virtual void OnDetach() {};
  virtual void Update() {};
  virtual void FixedUpdate(f32 DeltaTime) {};
  virtual void OnEvent(const Event &event) {};
};

class LayerStack {
public:
  ~LayerStack() {
    for (ApplicationLayer *layer : m_Layers) {
      layer->OnDetach();
      delete layer;
    }
  }

  inline void Push(ApplicationLayer *layer) {
    m_Layers.push_back(layer);
  }

  inline ApplicationLayer *Pop() {
    ApplicationLayer *layer = *m_Layers.rbegin();
    m_Layers.pop_back();
    return layer;
  }

  inline void Remove(ApplicationLayer *layer) {
    std::vector<ApplicationLayer *>::iterator it = std::find(m_Layers.begin(), m_Layers.end(), layer);
    if (it != m_Layers.end()) {
      layer->OnDetach();
      m_Layers.erase(it);
    }
  }

  inline std::vector<ApplicationLayer *>::iterator begin() { return m_Layers.begin(); }
  inline std::vector<ApplicationLayer *>::iterator end() { return m_Layers.end(); }

private:
  std::vector<ApplicationLayer *> m_Layers;
};

} // namespace Stak
