#pragma once

#include "Base.h"
namespace Stak
{
	class STAK_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual STKvoid OnAttach() {}
		virtual STKvoid OnDetach() {}
		virtual STKvoid OnUpdate() {}
		virtual STKvoid OnImGuiRender() {}
		
		inline const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};
}