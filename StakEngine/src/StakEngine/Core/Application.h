#pragma once

#include "Base.h"

namespace Stak
{
	class STAK_API Application
	{
	public:
		Application() {}
		virtual ~Application();

		STKvoid Run();
	};
	Application* CreateApplication();
}