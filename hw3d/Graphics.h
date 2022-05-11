#pragma once
#include "CleanWin.h"
#include <d3d11.h>
#include "ChiliException.h"
#include <vector>
#include "DxgiInfoManager.h"

class Graphics
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException; //Allows use of base class function
	};
	//---
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {} ) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	//---
	class DeviceRemovedException : public HrException //Specialised exception, distinct from HrException 
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete; //No copy constructor
	Graphics& operator=(const Graphics&) = delete; //No = operator
	~Graphics();
	void EndFrame(); //Present: flip back buffer
	void ClearBuffer(float red, float green, float blue) noexcept;
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager; //Only include if building in debug mode 
#endif
	//Pointers to hold D3D11 handles
	ID3D11Device* pDevice = nullptr; //Controls Windows setup for graphics hardware
	IDXGISwapChain* pSwap = nullptr; //Swapchain holds front/back buffer - DXGI layer includes things that don't change much over time, swap chain is solid theoretically and can't 'improve' or change much so is part of DXGI instead of a particular D3Dxx implementation
	ID3D11DeviceContext* pContext = nullptr; //Controls rendering & binding to pipeline
	ID3D11RenderTargetView* pTarget = nullptr; //Backbuffer resource - write new frames to this	
};

