#include "stkpch.h"

#include "StakEngine/Core/STK_ASSERT.h"
#include "WindowsWindow.h"
#include "StakEngine/Events/ApplicationEvent.h"
#include "StakEngine/Events/KeyEvent.h"
#include "StakEngine/Events/MouseEvent.h"

namespace Stak
{
	static uint8_t s_GLFWWindowCount = 0;

	WindowsWindow::WindowsWindow(const WindowProperties& properties)
	{
		Init(properties);
	}
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}
	STKvoid WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
	STKvoid WindowsWindow::SetVSync(STKbool enabled)
	{
		glfwSwapInterval(enabled);
		m_Data.VSync = enabled;
	}
	STKvoid WindowsWindow::Init(const WindowProperties& properties)
	{
		m_Data.Title = properties.Name;
		m_Data.Width = properties.Width;
		m_Data.Height = properties.Height;
		m_Data.VSync = true;

		STK_CORE_INFO("Creating Window {0} ({1}, {2})", properties.Name, properties.Width, properties.Height);

		if (s_GLFWWindowCount == 0)
		{
			int success = glfwInit();
			STK_CORE_ASSERT(success, "Failed to initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), NULL, NULL);
		s_GLFWWindowCount++;
		glfwMakeContextCurrent(m_Window);

		int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		STK_CORE_ASSERT(success, "Failed to initialize GLAD");

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(m_Data.VSync);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, STKint width, STKint height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, STKint key, STKint scancode, STKint action, STKint mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, false);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, true);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, STKuint keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});
	}
	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		s_GLFWWindowCount--;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}
	void GLFWErrorCallback(STKint error, const STKchar* description)
	{
		STK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}
}