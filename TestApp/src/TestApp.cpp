#include <StakEngine.h>

#include <Glad/glad.h>
#include <GLFW/glfw3.h>

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