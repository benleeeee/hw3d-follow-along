#pragma once
#include "CleanWin.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
	//Exception class inherit from ChiliException
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept; //Construct also includes windows HRESULT which is an error code
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
	// singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete; //delete copy constructor to ensure no copies of singleton
		WindowClass& operator=(const WindowClass&) = delete; //delete = operator to ensure singleton is not changed
		static constexpr const char* wndClassName = "Chili Direct3D Engine Window"; //const name
		static WindowClass wndClass; //static singleton inst of WindowClass
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name) ; //Constructor
	~Window(); //Destructor
	Window(const Window&) = delete; //ensure no copy constructor can be called
	Window& operator=(const Window&) = delete; //ensure no = operator can be used
	void SetTitle(const std::string& title);
private:
	//These functions are so we can use HandleMsg() as a member function to handle win messages
	//Without this workout you cannot call HandleMsg directly as a window procedure
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	
public:	
	Keyboard kbd;	//Keyboard obj to handle key and char events
	Mouse mouse;	//Mouse obj to handle mouse input events
private:
	int width;
	int height;
	HWND hWnd;	//WinAPI window class
};


// error exception helper macro that gets the file and line number for exceptions
#define CHWND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define CHWND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() ) //Some windows don't throw errors, so use GetLastError 