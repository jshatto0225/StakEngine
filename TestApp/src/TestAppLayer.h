#pragma once
#include "StakEngine/Core/Layer.h"

class TestAppLayer : public Stak::Layer
{
public:
	TestAppLayer();
	virtual ~TestAppLayer();
	void OnAttach();
	void OnDetach();
	void OnUpdate();
	void OnImGuiRender();
};