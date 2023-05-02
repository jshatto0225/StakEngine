#pragma once

#include "Base.h"
#include "layer.h"

namespace Stak
{
	class STAK_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		STKvoid PushLayer(Layer* layer);
		STKvoid PushOverlay(Layer* overlay);
		STKvoid PopLayer(Layer* layer);
		STKvoid PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		STKuint m_LayerInsertIndex = 0;
	};
}