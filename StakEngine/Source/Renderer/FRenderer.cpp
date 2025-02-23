#include "FRenderer.h"

//#include "IRHIInstance.h"
//#include "IRHIDevice.h"
//#include "IRHIContext.h"
//#include "IRHIResource.h"
#include "IWindow.h"
#include "RHI.h"
#include "FApplication.h"

Renderer::Renderer() {
    ASSERT(!instance);
    instance = this;
}

Renderer::~Renderer() {

}

void Renderer::render() {
    command_list.begin_drawing();
    {
        for (Render_Proxy *proxy : render_proxies) {
            proxy->render(command_list);
        }
        for (Render_Proxy *proxy : post_render_proxies) {
            proxy->render(command_list);
        }
    }
    command_list.end_drawing();
    
    Rhi::get().submit(command_list);
}

void Renderer::init_imgui() {
    App::get().get_window()->init_imgui();
    Rhi::get().init_imgui();
}

void Renderer::imgui_new_frame() {
    App::get().get_window()->imgui_new_frame();
    Rhi::get().imgui_new_frame();
}

void Renderer::shutdown_imgui() {
    Rhi::get().shutdown_imgui();
    App::get().get_window()->shutdown_imgui();
}

void Renderer::add_proxy(Render_Proxy *proxy) {
    render_proxies.push_back(proxy);
}

void Renderer::remove_proxy(Render_Proxy *proxy) {
    auto it = std::find(render_proxies.begin(), render_proxies.end(), proxy);
    if (it != render_proxies.end()) {
        render_proxies.erase(it);
    }
}

void Renderer::add_post_proxy(Render_Proxy *proxy) {
    post_render_proxies.push_back(proxy);
}

void Renderer::remove_post_proxy(Render_Proxy *proxy) {
    auto it = std::find(post_render_proxies.begin(), post_render_proxies.end(), proxy);
    if (it != post_render_proxies.end()) {
        post_render_proxies.erase(it);
    }
}

Render_Proxy::Render_Proxy() {
    Renderer::get().add_proxy(this);
}

Render_Proxy::~Render_Proxy() {
    Renderer::get().remove_proxy(this);
}
