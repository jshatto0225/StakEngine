#pragma once

#include "StakEngine/Core/base.h"

#include "Platform/Windows/WindowsWindow.h"
#include "StakEngine/Core/LayerStack.h"
#include "StakEngine/Events/Event.h"
#include "StakEngine/Events/ApplicationEvent.h"

namespace Stak
{
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		STKvoid OnEvent(Event& e);

		STKvoid Run();

		STKvoid PushLayer(Layer* layer);
		STKvoid PushOverlay(Layer* overlay);

		Window& Getindow() { return *m_Window; }

		Application* Get() { return s_Instance; }

	private:
		STKbool OnWindowClose(WindowCloseEvent& e);
		STKbool OnWindowResize(WindowResizeEvent& e);

	private:
		STKbool m_Running = true;
		LayerStack m_LayerStack;
		WindowsWindow* m_Window;
		static Application* s_Instance;
	};
	Application* CreateApplication();
}