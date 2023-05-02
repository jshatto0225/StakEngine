#include "stkpch.h"
#include "Application.h"

namespace Stak
{
	STKvoid Application::Run()
	{
		while (m_Running);
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			// Initialize ImGui
			/*for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();*/

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
	STKbool Application::OnWindowClose()
	{
		m_Running = false;
		return true;
	}
}