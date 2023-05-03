#pragma once

#include <StakEngine.h>

class TestAppLayer : public Stak::Layer
{
public:
	TestAppLayer();
	virtual ~TestAppLayer();
	void OnAttach();
	void OnDetach();
	void OnUpdate();
	void OnImGuiRender();
	void OnEvent(Stak::Event& e);
};
