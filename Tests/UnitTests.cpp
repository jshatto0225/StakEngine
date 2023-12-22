#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define SK_NO_MAIN
#include "Runtime/Runtime.h"

TEST_CASE("Test Log", "[Log]")
{
    REQUIRE_THROWS(SK_CORE_INFO("throws"));
    Log::Init();
    REQUIRE_NOTHROW(SK_CORE_INFO("nothrow"));
    Log::Shutdown();
    REQUIRE_THROWS(SK_CORE_INFO("throws"));
}

TEST_CASE("Window Test", "[Window]")
{
    REQUIRE_NOTHROW(Application::Init());

    Shared<Window> window = PlatformManager::NewWindow("TempWindow", 0, 0, 800, 450);
    REQUIRE(window->IsOpen());
    REQUIRE(window->GetWidth() == 800);
    REQUIRE(window->GetHeight() == 450);
    REQUIRE(window->GetAspect() == 16.0f / 9.0f);
    REQUIRE(window->GetX() == 0);
    REQUIRE(window->GetY() == 0);
    window->SetWidth(450);
    REQUIRE(window->GetWidth() == 450);
    REQUIRE(window->GetAspect() == 1);
    window->SetHeight(800);
    REQUIRE(window->GetHeight() == 800);
    REQUIRE(window->GetAspect() == 9.0f / 16.0f);
    REQUIRE(window->GetId() == 1);
    window->SetX(200);
    REQUIRE(window->GetX() == 200);
    window->SetY(200);
    REQUIRE(window->GetY() == 200);
    window->Close();
    window->SetSizeAndPos(500, 500, 100, 200);
    REQUIRE(window->GetWidth() == 100);
    REQUIRE(window->GetHeight() == 200);
    REQUIRE(window->GetAspect() == 0.5);
    REQUIRE(window->GetX() == 500);
    REQUIRE(window->GetY() == 500);
    REQUIRE_FALSE(window->IsOpen());

    REQUIRE_NOTHROW(Application::Shutdown());
}

TEST_CASE("Test Layers", "[ApplicationLayer]")
{
    static bool layerStarted = false;
    static bool layerEnded = false;
    static bool layerUpdated = false;

    class AppTestLayer : public ApplicationLayer
    {
    public:
        void Start() override
        {
            layerStarted = true;
        }

        void End() override
        {
            layerEnded = true;
        }

        void Update() override
        {
            layerUpdated = true;
            Application::Shutdown();
        }
    };

    REQUIRE_NOTHROW(Application::RegisterLayer<AppTestLayer>());
    REQUIRE_NOTHROW(Application::Init());
    REQUIRE_NOTHROW(Application::Run());
    REQUIRE(layerStarted == true);
    REQUIRE(layerEnded == true);
    REQUIRE(layerUpdated == true);
}

TEST_CASE("Test Event", "[Application::OnEvent]")
{
    static bool eventRecieved = false;

    class EventTestLayer : public ApplicationLayer
    {
    public:
        void Update() override
        {
            Event e = { TEST_EVENT };
            Application::OnEvent(e);
            Application::Shutdown();
        }

        void OnEvent(Event& e)
        {
            if (e.type == TEST_EVENT)
            {
                eventRecieved = true;
            }
        }
    };

    REQUIRE_NOTHROW(Application::RegisterLayer<EventTestLayer>());
    REQUIRE_NOTHROW(Application::Init());
    REQUIRE_NOTHROW(Application::Run());
    REQUIRE(eventRecieved == true);
}

TEST_CASE("General Application Test", "[Application]")
{
    class ShutdownTestLayer : public ApplicationLayer
    {
    public:
        void Update() override
        {
            Application::Shutdown();
        }
    };

    REQUIRE_NOTHROW(Application::RegisterLayer<ShutdownTestLayer>());
    REQUIRE_NOTHROW(Application::Shutdown());
    REQUIRE_NOTHROW(Application::Run());
    REQUIRE_NOTHROW(Application::Init());
    REQUIRE_NOTHROW(Application::Run());
}
