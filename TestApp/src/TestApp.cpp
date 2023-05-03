#include <StakEngine.h>
#include <StakEngine/Core/EntryPoint.h>

#include "TestAppLayer.h"

class TestApp : public Stak::Application
{
public:
	TestApp()
	{
		PushLayer(new TestAppLayer());
	}
	virtual ~TestApp()
	{
	}
};

Stak::Application* Stak::CreateApplication()
{
	return new TestApp();
}