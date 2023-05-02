#include <StakEngine.h>

class TestApp : public Stak::Application
{
public:
	TestApp()
	{
	}
	virtual ~TestApp()
	{
	}
};

Stak::Application* Stak::CreateApplication()
{
	return new TestApp();
}