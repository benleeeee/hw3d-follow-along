#pragma once
#include "Window.h"
#include <sstream>
#include "ChiliTimer.h"
#include "ImguiManager.h"

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
	bool show_demo_window = true;
	static constexpr size_t nDrawables = 180;
};