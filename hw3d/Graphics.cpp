#include "Graphics.h"

//Set the linker settings for d3d11 library, necessary to link to actual library functions
#pragma comment(lib,"d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	//Create swap chain descriptor
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1; //Count/quality set to no anti-aliasing
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //pipeline renders to this buffer target
	sd.BufferCount = 1; //1 front buffer, one back buffer
	sd.OutputWindow = hWnd; //<--- Pass in hWnd to render on
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//Create device, swap chain and rendering context in one
	D3D11CreateDeviceAndSwapChain(
		nullptr,	//choose default adapter
		D3D_DRIVER_TYPE_HARDWARE, //Use hardware driver 
		nullptr,	//
		0,			//
		nullptr,	//no feature levels specified GPU needs to support
		0,			//
		D3D11_SDK_VERSION,	//sdk version targetting (which D3D needs to be installed
		&sd,		//descriptor structure for configuring swap chain
		&pSwap,		//fills in ptr to swap chain
		&pDevice,	//fills in ptr to device
		nullptr,	//
		&pContext	//fills in ptr to context
	);

	//Gain access to texture subresource in swap chain (backbuffer)
	ID3D11Resource* pBackBuffer = nullptr;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
	pDevice->CreateRenderTargetView(
		pBackBuffer,
		nullptr,
		&pTarget
	);
	//BackBuff resource only needed to fill pTarget, so now we can release the handle
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	//RAII (Resource acquisition is initialised)
	if (pTarget != nullptr)
	{
		delete(pTarget);
	}
	if (pContext != nullptr)
	{
		delete(pContext);
	}
	if (pSwap != nullptr)
	{
		delete(pSwap);
	}
	if (pDevice != nullptr)
	{
		delete(pDevice);
	}			
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}
