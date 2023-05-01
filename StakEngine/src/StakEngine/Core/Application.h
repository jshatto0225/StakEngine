#pragma once

namespace Stak
{
	class Application
	{
	public:
		Application() {}
		virtual ~Application();

		void Run();
	};
	Application* CreateApplication();
}