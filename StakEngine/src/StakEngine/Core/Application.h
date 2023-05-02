#pragma once

#include "Base.h"

#include "LayerStack.h"

namespace Stak
{
	class STAK_API Application
	{
	public:
		Application() {}
		virtual ~Application() = default;

		STKvoid Run();

		STKvoid PushLayer(Layer* layer);
		STKvoid PushOverlay(Layer* overlay);

		STKbool OnWindowClose();
	private:
		STKbool m_Running = true;
		LayerStack m_LayerStack;
	};
	Application* CreateApplication();
}