#include "WindowManager.h"

#include "PlatformManager.h"
#include "Log.h"
#include "Renderer.h"

std::unordered_map<u64, Unique<Window>> WindowManager::sIdWindowMap;
bool WindowManager::sRunning;

void WindowManager::Init()
{
    sRunning = true;
}

void WindowManager::Shutdown()
{
    if(sRunning)
    {
        sRunning = false;
        for(const auto& idWindowPair : sIdWindowMap)
        {
            idWindowPair.second->Close();
        }
        sIdWindowMap.clear();
    }
}

void WindowManager::Update()
{
    if(sRunning)
    {
        for(const auto& idWindowPair : sIdWindowMap)
        {
            idWindowPair.second->Update();
        }
    }
}

u64 WindowManager::AddWindow(const std::string& name, i32 x, i32 y, i32 width, i32 height)
{
    if(sRunning)
    {
        Unique<Window> newWindow = PlatformManager::NewWindow(name, x, y, width, height);
        u64 newId = newWindow->GetId();
        sIdWindowMap.insert({ newWindow->GetId(), std::move(newWindow) });
        return newId;
    }
    return 0;
}

bool WindowManager::HasWindow(u64 id)
{
    if(!sRunning)
    {
        return false;
    }
    return sIdWindowMap.count(id) != 0;
}

void WindowManager::SetWindowX(u64 id, i32 x)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return;
    }
    sIdWindowMap[id]->SetX(x);
}

void WindowManager::SetWindowY(u64 id, i32 y)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return;
    }
    sIdWindowMap[id]->SetY(y);
}

void WindowManager::SetWindowWidth(u64 id, i32 width)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return;
    }
    sIdWindowMap[id]->SetWidth(width);
}

void WindowManager::SetWindowHeight(u64 id, i32 height)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return;
    }
    sIdWindowMap[id]->SetHeight(height);
}

i32 WindowManager::GetWindowWidth(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
   return sIdWindowMap[id]->GetWidth();
}

i32 WindowManager::GetWindowHeight(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
    return sIdWindowMap[id]->GetHeight();
}

i32 WindowManager::GetWindowX(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
    return sIdWindowMap[id]->GetX();
}

i32 WindowManager::GetWindowY(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
    return sIdWindowMap[id]->GetY();
}

f32 WindowManager::GetWindowAspect(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
    return sIdWindowMap[id]->GetAspect();
}

bool WindowManager::IsWindowOpen(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
    return sIdWindowMap[id]->IsOpen();
}

void WindowManager::SetWindowSizeAndPos(u64 id, i32 x, i32 y, i32 width, i32 height)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return;
    }
    sIdWindowMap[id]->SetSizeAndPos(x, y, width, height);
}

void* WindowManager::GetWindowHandle(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return 0;
    }
    if(id == 0)
    {
        return 0;
    }
    return sIdWindowMap[id]->GetHandle();
}

void WindowManager::CloseWindow(u64 id)
{
    if(!HasWindow(id) || !sRunning)
    {
        SK_CORE_WARN("Window does not exist");
        return;
    }
    sIdWindowMap[id]->Close();
    sIdWindowMap.erase(id);
}
