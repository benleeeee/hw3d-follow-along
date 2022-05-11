#include "Graphics.h"
#include "dxerr.h"
#include <sstream>

//Set the linker settings for d3d11 library, necessary to link to actual library functions
#pragma comment(lib,"d3d11.lib")

//### Macros for easier typing to throw exceptions ###
// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
//creates specialised deviceRemovedException, which can be thrown manually
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#endif



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

	//Create default flags UINT, if building for debug use |= operator to combine the flag for creating on device debug layer
	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


	//for checking results of d3d function
	HRESULT hr;	

	//Create device, swap chain and rendering context in one
	GFX_THROW_INFO( D3D11CreateDeviceAndSwapChain(
		nullptr,	//choose default adapter
		D3D_DRIVER_TYPE_HARDWARE, //Use hardware driver 
		nullptr,	//
		swapCreateFlags, // Create device on debug layer so we get debug info in VS error log
		nullptr,	//no feature levels specified GPU needs to support
		0,			//
		D3D11_SDK_VERSION,	//sdk version targetting (which D3D needs to be installed
		&sd,		//descriptor structure for configuring swap chain
		&pSwap,		//fills in ptr to swap chain
		&pDevice,	//fills in ptr to device
		nullptr,	//
		&pContext	//fills in ptr to context
	) );

	//Gain access to texture subresource in swap chain (backbuffer)
	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_INFO( pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer)) );
	GFX_THROW_INFO( pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget) );
	
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
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	//Check for special h-result error from Present() 
	if ( FAILED(hr = pSwap->Present(1u, 0u)) )
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT( pDevice->GetDeviceRemovedReason() );
		}
		else
		{
			GFX_THROW_INFO(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float colour[] = { red, green, blue, 1.0f };
	//Clear render target (backbuffer) into an RGB colour using Context which controls rendering
	pContext->ClearRenderTargetView(pTarget, colour);
}

//####################################
//##### Graphics Exception Stuff #####
//####################################
const char * Graphics::DeviceRemovedException::GetType() const noexcept	
{
	return "Chili Graphics Execption [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::HrException::HrException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs ) noexcept
	:
	Exception( line, file ), //Use 'Exception' constructor which is an alias for ChiliException constructor
	hr(hr)	//Initilaise HRESULT stored in Graphics::Exception::HrException
{
	//Join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	//Remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char * Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorDescription() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << Graphics::HrException::GetErrrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();

}

const char * Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	//DXG lib macro to parse h-result error description into char buffer
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrrorInfo() const noexcept
{
	return info;
}
