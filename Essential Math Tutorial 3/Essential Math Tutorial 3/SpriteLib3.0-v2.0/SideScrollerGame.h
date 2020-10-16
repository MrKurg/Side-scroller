#pragma once

#include "Scene.h"

class SideScrollerGame : public Scene
{
public:
	SideScrollerGame(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	float timer;
	bool canJump = false;
	float maxSpeed = 150.f;

	//Input overrides
	void KeyboardHold() override;
	void KeyboardDown() override;
	void KeyboardUp() override;

protected:

};
