#include "stkpch.h"
#include "Application.h"

namespace Stak
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		m_Window = new WindowsWindow();
		m_Window->SetEventCallback(STK_BIND_EVENT_FN(OnEvent));
		s_Instance = this;
	}

	STKvoid Application::Run()
	{
		while (m_Running);
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_Window->OnUpdate();
		}
	}

	STKvoid Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(STK_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(STK_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto position = m_LayerStack.begin(); position != m_LayerStack.end(); position++)
		{
			if (e.Handled)
				break;
			(*position)->OnEvent(e);
		}
	}

	STKvoid Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	STKvoid Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
	}

	STKbool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	STKbool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			return false;
		}

		// Tell renderer to set viewport
		return false;
	}
}