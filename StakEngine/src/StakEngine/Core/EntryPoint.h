#pragma once

#include "Base.h"
#include "Application.h"

extern Stak::Application* Stak::CreateApplication();

STKint main()
{
	Stak::Log::Init();
	STK_CORE_WARN("Initialized Logger");

	auto app = Stak::CreateApplication();
	STK_INFO("Started Application");
	app->Run();
}