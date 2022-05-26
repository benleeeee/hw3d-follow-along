#pragma once
#include "CleanWin.h"
#include <d3d11.h>
#include "ChiliException.h"
#include <wrl.h>
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
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info; //only stores info not Hresult		
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
	~Graphics() = default; //c++ 11: specify to use compiler-generated destructor
	void EndFrame(); //Present: flip back buffer
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTestTriangle(float angle, float x, float y);
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager; //Only include if building in debug mode 
#endif
	//WRL COM Pointers to hold D3D11 handles, these auto delete when they go out of scope
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice; //Controls Windows setup for graphics hardware
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap; //Swapchain holds front/back buffer - DXGI layer includes things that don't change much over time, swap chain is solid theoretically and can't 'improve' or change much so is part of DXGI instead of a particular D3Dxx implementation
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext; //Controls rendering & binding to pipeline
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> pTarget; //Backbuffer resource - write new frames to this	
};

