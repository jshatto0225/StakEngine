#include "Runtime/Runtime.h"

class TestLayer : public ApplicationLayer
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
    Shared<Window> mWindow;
};

void ExternInit()
{
    Application::RegisterLayer<TestLayer>();
}
