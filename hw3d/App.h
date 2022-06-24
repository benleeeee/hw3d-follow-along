#pragma once
#include "Window.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Camera.h"

class App
{
public:
	App();
	~App(); //Don't need a constructor, it won't be destroyed until whole app is closed?

	//master frame / message loop
	int Go();

private:
	void DoFrame();
private:
	ImguiManager imgui;
	Window wnd;
	ChiliTimer timer; 
	std::vector<std::unique_ptr<class Drawable>> drawables;
	float sim_speed = 1.0f;
	Camera cam;
	static constexpr size_t nDrawables = 180;
};