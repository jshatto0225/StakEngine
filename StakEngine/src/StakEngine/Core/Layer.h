#pragma once

#include "StakEngine/Core/Base.h"
#include "StakEngine/Events/Event.h"

namespace Stak
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual STKvoid OnAttach() {}
		virtual STKvoid OnDetach() {}
		virtual STKvoid OnUpdate() {}
		virtual STKvoid OnImGuiRender() {}
		virtual STKvoid OnEvent(Event& e) {}
		
		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}