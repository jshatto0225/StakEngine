#include "Runtime/Runtime.h"

class TestLayer : public SK::ApplicationLayer
{
public:
    void Start()
    {
        SK_CORE_TRACE("Source dir: %s", SOURCE_DIR);
    }

    void Update()
    {

    }

    void OnEvent(Event& e)
    {

    }

private:
    Shared<SK::Window> mWindow;
};

void ExternInit()
{
    SK::Application::RegisterLayer<TestLayer>();
}
