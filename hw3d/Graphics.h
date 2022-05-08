#pragma once
#include "CleanWin.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete; //No copy constructor
	Graphics& operator=(const Graphics&) = delete; //No = operator
	~Graphics();
	void EndFrame(); //Present: flip back buffer
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		const float colour[] = { red, green, blue, 1.0f };		
		//Clear render target (backbuffer) into an RGB colour using Context which controls rendering
		pContext->ClearRenderTargetView(pTarget, colour);
	}

private:
	//Pointers to hold D3D11 handles
	ID3D11Device* pDevice = nullptr; //Controls Windows setup for graphics hardware
	IDXGISwapChain* pSwap = nullptr; //Swapchain holds front/back buffer - DXGI layer includes things that don't change much over time, swap chain is solid theoretically and can't 'improve' or change much so is part of DXGI instead of a particular D3Dxx implementation
	ID3D11DeviceContext* pContext = nullptr; //Controls rendering & binding to pipeline
	ID3D11RenderTargetView* pTarget = nullptr; //Backbuffer resource - write new frames to this	
};

