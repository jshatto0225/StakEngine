#include "stkpch.h"
#include "LayerStack.h"

namespace Stak
{
	LayerStack::LayerStack()
	{
	}
	LayerStack::~LayerStack()
	{
	}
	STKvoid LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
		layer->OnAttach();
	}
	STKvoid LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
		overlay->OnAttach();
	}
	STKvoid LayerStack::PopLayer(Layer* layer)
	{
		auto location = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
		if (location != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(location);
			m_LayerInsertIndex--;
		}
	}
	STKvoid LayerStack::PopOverlay(Layer* overlay)
	{
		auto location = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, overlay);
		if (location != m_Layers.begin() + m_LayerInsertIndex)
		{
			overlay->OnDetach();
			m_Layers.erase(location);
		}
	}
}
