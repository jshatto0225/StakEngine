#pragma once

#include <vector>

#include "Events.h"

class App_Layer {
public:
    virtual ~App_Layer() = default;

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void update() {}
    virtual void fixed_update(f32 delta_time) {}
    virtual void on_window_resize(const Window_Resize_Event &event) {}
    virtual void on_imgui_render() {}
};

class Layer_Stack {
public:
    ~Layer_Stack() {
        clear();
    }

    inline void clear() {
        while (!layers.empty()) {
            delete pop();
        }
    }

    inline void push(App_Layer *layer) {
        layers.push_back(layer);
        layer->on_attach();
    }

    inline App_Layer *pop() {
        App_Layer *layer = *layers.rbegin();
        layers.pop_back();
        layer->on_detach();
        return layer;
    }

    inline void remove(App_Layer * layer) {
        auto it = std::find(layers.begin(), layers.end(), layer);
        if (it != layers.end()) {
            layer->on_detach();
            layers.erase(it);
            delete layer;
        }
    }

    inline std::vector<App_Layer *>::iterator begin() { return layers.begin(); }
    inline std::vector<App_Layer *>::iterator end() { return layers.end(); }

private:
    std::vector<App_Layer *> layers;
};
