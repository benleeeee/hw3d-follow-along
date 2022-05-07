#include "Window.h"
#include <sstream>
#include "resource.h"

//Window Class stuff
Window::WindowClass Window::WindowClass::wndClass; //Singleton instance(?)

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle( nullptr))   //<---- this is 'member initialiser list', it does what its named...
										//		in this line hInst is set to the return of GetModuleHandle
										//		which is WinAPIs func for getting the handle to the instance?
{
	//Register windows class -- see https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassexa
	WNDCLASSEX wc = { 0 }; //good practise to 'zero out' a descriptor by default
	wc.cbSize = sizeof(wc);  //set size to size of structure -- idt you would ever do otherwise?
	wc.style = CS_OWNDC; //CS_OWNDC style gives every window it's own device context meaning they can be drawn to separately
	wc.lpfnWndProc = HandleMsgSetup; 
	wc.cbClsExtra = 0; //set 0 extra bytes of mem in the window class structure on the APIs side
	wc.cbWndExtra = 0; //set 0 extra bytes of mem in each instance of window created from this class
	wc.hInstance = GetInstance(); //instance passed into WinMain func
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); //load icon from resources
	wc.hCursor = nullptr; //default cursor
	wc.hbrBackground = nullptr; //no background drawing by win API, all drawing will be D3DX
	wc.lpszMenuName = nullptr; //no menu name
	wc.lpszClassName = GetName(); //important! sets up the class name for window on APIs side
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int width, int height, const char * name) 
	: width(width),
	height(height)
{
	//Calcualte window size based on desired client region size
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	//Perform exception checks on AdjustWindowRect
	//Adjusts the wr size to be in terms of client window (ignoring the side/top bars)	
	if (AdjustWindowRect(&wr,							//Pass in reference to wr for window size
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	//bitmasks(?) or bitfields(?) to style window
			FALSE)										//not a menu for some reason
					== 0 )
	{
		throw CHWND_LAST_EXCEPT();
	}


	//Create window & get hWnd
	hWnd = CreateWindow(
		WindowClass::GetName(), name, //Class name then window name
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	//Window styles
		CW_USEDEFAULT, CW_USEDEFAULT,	//Use [Client Window] default x,y start pos
		wr.right - wr.left, wr.bottom - wr.top, //Width and height, I have no idea why .left and .top needed to be set to anything other than 0, or why they need to be subtracted?
		nullptr,	//window handle to parent (there is no parent so set to null(?)
		nullptr,	//handle to menu also nullptr
		WindowClass::GetInstance(),	//handle to this windows instance passed in
		this	//pass in pointer to Window class that is creating the window
	);
	
	//Check for error creating window
	if (hWnd == nullptr)
	{	
		throw CHWND_LAST_EXCEPT();
	}
	//Show window
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{
	//SetWindowText returns bool, true/1 if set, false/0 if failed
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

//Called first time on new Window to set up framework for handling messages through HandleMsgThunk
LRESULT Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//use create parameter passed in from CreateWindow() to store window class pointer
	if (msg == WM_NCCREATE)
	{
		//extract ptr to window class that was passed in to lParam as 'this' during constructor of Window
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//set WinAPI-managed user data to store ptr to 'Window' class, this creates a link between winAPI side and the class that we use to control the window (winAPI has a ptr to the Window class)
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//set message proc to normal (non-setup) handler 'HandleMsgThunk' now that setup is finished
		//this means that HandleMsgSetup has done its not and no longer needs to be called, so call HandleMsgThunk instead
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		//forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	//if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc(hWnd, msg, wParam, lParam); 
}

//Retrieves the ptr to Window class from the custom data stored in Win32 side so we can invoke the HandleMsg function
//so it can be used as a wrapper to handling each message
LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);	
}


//Due to the other two HandleMsg... functions acting as workarounds, we can now access all of the member data of window
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		//On WM_CLOSE post quit message but then return 0 code so default winproc doesn't destroy window
		//since it will be destroyed already by ~Window(). This is handled in WinMain message pump when 
		//the code 0 exits the pump and the window is destroyed
		PostQuitMessage(0);
		return 0;

	case WM_KILLFOCUS:
		//On WM_KILLFOCUS the window loses focus (it is not the primary selected window)
		//Which means any keyup or other events don't go into this window. So it's a good idea
		//to flush out the keybuffer
		kbd.ClearState(); //<-- reset all bitflags for each key 
		break;

		/***************** KEYBOARD MESSAGES ********************/
		//fall-through execute for KEYDOWN and SYSKEYDOWN, the latter is to handle ALT key & other system keys like F10
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN: 
		if (!(lParam & 0x40000000) //As per microsoft docs, lParam contains much info packed into bits, the 30th bit is checked using the bitmask 0x40000000, if false then the key was NOT pressed the last frame
			|| kbd.AutorepeatIsEnabled()) //OR allow key auto repeat
		{
			//At this point it's either the first keypress of a long hold, or autorepeat lets the hold continually proc
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/*************** END KEYBOARD MESSAGES ******************/


		/***************** MOUSE MESSAGES ********************/
	case WM_MOUSEMOVE:
	{   
		const POINTS pt = MAKEPOINTS(lParam); //lParam holds mouse coords from windows
		//in client region-> log move & log ener + capture mouse (if not previously)
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.Mouse::OnMouseMove(pt.x, pt.y);//Pass windows provided coords to mouse class		

			//if not prev in window, set mouse to capture & update state
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd); //<-- winAPI to keep capturing mouse
				mouse.OnMouseEnter();
			}
		}
		//not in client-> log move / maintain capture only IF button down
		else
		{
			if (wParam * (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			//button up -> release capture/log event for leaving
			else
			{
				ReleaseCapture(); //<--winAPI to release mouse
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.Mouse::OnLeftPressed(5, 5);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.Mouse::OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.Mouse::OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.Mouse::OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{	
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta); 
		break;
	}
		/*************** END MOUSE MESSAGES ******************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Exception::Exception(int line, const char * file, HRESULT hr) noexcept
	:
	ChiliException (line, file),	//Invoke ChiliException constructor in initialiser list for line/file param
	hr(hr)			//since the other constructor doesn't deal with HRESULT param, initialise here
{
}

const char * Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Window::Exception::GetType() const noexcept
{
	return "Chili Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	//ptr to point to allocated buffer for output of FormatMessage called below
	char* pMsgBuf = nullptr;
	//Call FormatMessage which gives description string for a HRESULT err code into a buffer in mem, the return value is the length of the output string
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |	//Allocate a buffer for output automatically
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,	//Styling the format
		nullptr,
		hr,	//<<--- error code
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf),	//pass in ptr to a pointer, it will make pMsgBuf now point to the allocated buffer w/ results
		0,
		nullptr
	);
	
	if (nMsgLen == 0)
		return "Unidentified error code";

	//Pull contents of buffer into string to return out function
	std::string errorString = pMsgBuf;

	//Need to free the windows allocated buffer now we have results in errorString
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}