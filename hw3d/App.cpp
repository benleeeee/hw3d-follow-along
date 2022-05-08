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
		if (const auto ecode = Window::ProcessMessage())
		{
			//if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float t = timer.Peek();
	std::ostringstream oss;
	oss << "Time elapsed: " << std::setprecision(1) << std::fixed << t << "s";
	wnd.SetTitle(oss.str());			
}

