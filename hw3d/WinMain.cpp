#include "CleanWin.h"
#include "WindowsMessageMap.h"
#include <sstream>
#include "App.h"

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
		//Create app that contains window & game logic
		return App{}.Go();
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