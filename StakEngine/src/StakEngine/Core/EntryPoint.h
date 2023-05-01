#pragma once

#include "Base.h"
#include "Application.h"

extern Stak::Application* Stak::CreateApplication();

int main()
{
	auto app = Stak::CreateApplication();
	app->Run();
}