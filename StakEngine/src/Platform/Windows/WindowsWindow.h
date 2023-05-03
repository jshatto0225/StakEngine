#pragma once

#include "StakEngine/Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Stak
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProperties& properties = WindowProperties());
		virtual ~WindowsWindow();

		STKvoid OnUpdate() override;

		inline STKuint GetWidth() const override  { return m_Data.Width; };
		inline STKuint GetHeight() const override  { return m_Data.Height; };

		STKvoid SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		STKvoid SetVSync(STKbool enabled) override;
		inline STKbool IsVSync() const override { return m_Data.VSync; };
	private:
		virtual STKvoid Init(const WindowProperties& properties);
		virtual STKvoid Shutdown();

		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			STKuint Width;
			STKuint Height;

			STKbool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

	static void GLFWErrorCallback(STKint error, const STKchar* description);
}