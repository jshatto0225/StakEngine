#pragma once

#include <imgui.h>

#include "RHICommandList.h"
#include "IWindow.h"

class Render_Proxy {
public:
    Render_Proxy();
    virtual ~Render_Proxy();

    virtual void render(Rhi_Command_List &command_list) {}
};


class Renderer {
public:
    Renderer();
    ~Renderer();

    inline static Renderer &get() { return *instance; }

    void init_imgui();
    void imgui_new_frame();
    void shutdown_imgui();
    void render();

    void add_proxy(Render_Proxy *Proxy);
    void remove_proxy(Render_Proxy *Proxy);

    void add_post_proxy(Render_Proxy *Proxy);
    void remove_post_proxy(Render_Proxy *Proxy);

    Renderer(Renderer &) = delete;
    Renderer &operator=(Renderer &) = delete;

private:
    inline static Renderer *instance = NULL;

private:
    std::vector<Render_Proxy *> render_proxies;
    std::vector<Render_Proxy *> post_render_proxies;
    Rhi_Command_List command_list;
};
