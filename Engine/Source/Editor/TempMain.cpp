#include "Runtime/Runtime.h"

class TestLayer : public ApplicationLayer
{
public:
    void Start()
    {
        mWindow = WindowManager::AddWindow("NewWindow", 0, 0, 100, 100);
    }

    void OnEvent(Event& e)
    {
        if(e.type == WINDOW_CLOSE)
        {
            WindowCloseEvent* wce = (WindowCloseEvent*)&e;
            if(wce->windowId == mWindow)
            {
                Application::Shutdown();
            }
        }
    }

private:
    u64 mWindow;
};

void ExternInit()
{
    Application::RegisterLayer<TestLayer>();
}
