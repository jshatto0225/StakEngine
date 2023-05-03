#pragma once

#include "StakEngine/Core/Base.h"
#include "StakEngine/Events/Event.h"

#include <functional>

namespace Stak
{
	struct WindowProperties
	{
		std::string Name;
		STKuint Width;
		STKuint Height;

		WindowProperties(const std::string& name = "Stak Engine", STKuint width = 1280, STKuint height = 720)
			:Name(name), Width(width), Height(height)
		{
		}
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual STKvoid OnUpdate() = 0;

		virtual STKuint GetWidth() const = 0;
		virtual STKuint GetHeight() const = 0;

		virtual STKvoid SetEventCallback(const EventCallbackFn& callback) = 0;;
		virtual STKvoid SetVSync(STKbool enabled) = 0;
		virtual STKbool IsVSync() const = 0;
	};
}