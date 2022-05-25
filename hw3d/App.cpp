#include "App.h"
#include <iomanip>

App::App()
	:
	wnd(800, 600, "My Window") //Initialise the window w/ the app 
{
}

int App::Go()
{
	//As an app that continually runs it is no longer an event based program like a normal window
	while (true)
	{
		//Process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			//if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(0, 0, c); //Clear buffer and set to colour
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame(); //MUST call EndFrame to present backbuffer
}

