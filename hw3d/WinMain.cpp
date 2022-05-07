#include "CleanWin.h"
#include "WindowsMessageMap.h"
#include <sstream>
#include "Window.h"

#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif

//Custom defined windows procedure - this dictates how a window will deal with different message types
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Use static WindowsMessageMap obj to output debugging info on win message
	static WindowsMessageMap mm;
	OutputDebugString( mm(msg, lParam, wParam).c_str() );

	switch (msg)
	{
	case WM_CLOSE: //WM = windows message, so WM_CLOSE = window close
	{	
		PostQuitMessage(69);
		break;
	}
	case WM_KEYDOWN:
	{
		if (wParam == 'F')
		{
			//Just change name of window for now
			SetWindowText(hWnd, "Respects");
		}
		break;
	}
	case WM_KEYUP:
	{
		if (wParam == 'F')
		{
			SetWindowText(hWnd, "hw3d Window");
		}
		break;
	}
	case WM_CHAR:
	{
		static std::string title;
		title.push_back((char)wParam);
		SetWindowText(hWnd, title.c_str());
		break;
	}
		//Left mouse click
	case WM_LBUTTONDOWN: 
	{
		POINTS pt = MAKEPOINTS(lParam); // Make x,y point at mouse coords
		std::ostringstream oss; //outstringstream
		oss << "(" << pt.x << "," << pt.y << ")";
		SetWindowText(hWnd, oss.str().c_str());
		break;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//Any functions that the windows API will call must have the stdcall calling 
//convention set by the CALLBACK modifier. using __stdcall is equally valid
int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	try
	{
		//Create Window obj to handle creation of window neatly, when this obj is destroyed at end of WinMain()
		//the window instance will also be destroyed
		Window myWindow(192 * 4, 108 * 4, "My Window");

		//Event-based program, messages are queued up and dealt with in order to update the app, there is no constant update loop	

		//Message pump
		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0) //as long as the return value is >0 then keep processing. 0 = close, -1 = error	
		{
			//Application needs to translate the message and send back to win32 app
			TranslateMessage(&msg);

			//Pass message along to Window Proc (Procedure) which informs how to handle each message
			DispatchMessage(&msg);

			//
			static int upcount = 0;
			while( !myWindow.mouse.IsEmpty() )
			{
				const auto e = myWindow.mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::Leave:
					myWindow.SetTitle("Gone!");
					break;
				case Mouse::Event::Type::Move:
				{
					std::ostringstream oss;
					oss << "Mouse Position: (" << e.GetPosX() << "," << e.GetPosY() << ")";
					myWindow.SetTitle(oss.str());
					break;
				}
				case Mouse::Event::Type::WheelUp:
				{
					upcount++;
					std::ostringstream oss;
					oss << "Mouse scrolled up: " << upcount;
					myWindow.SetTitle(oss.str());
					break;
				}
				case Mouse::Event::Type::WheelDown:
				{
					upcount--;
					std::ostringstream oss;
					oss << "Mouse scrolled down: " << upcount;
					myWindow.SetTitle(oss.str());
					break;
				}
				}
			}
		}

		if (gResult == -1)
		{
			return -1;
		}


		return msg.wParam;	//.wParam will be set to the value passed into PostQuitMessage which must be called in order to close program
							//PostQuitMessage is called by the custom winProc when closing the app.
							//lParam and wParam are set differently based on the context of the message.
							//Google WM_QUIT and look at docs.microsoft documentations to find what these are set to
							//In this instance the WM_QUIT sets wParam = exit code given, and lParam is not used
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	//Try caught exception so return -1
	return -1;
}