#pragma once
#include "Window.h"
#include <sstream>
#include "ChiliTimer.h"

class App
{
public:
	App();
	//~App(); Don't need a constructor, it won't be destroyed until whole app is closed?

	//master frame / message loop
	int Go();

private:
	void DoFrame();
private:
	Window wnd;
	ChiliTimer timer; 
};