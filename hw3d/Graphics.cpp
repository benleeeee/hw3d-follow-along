#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

//Set the linker settings for d3d11 library, necessary to link to actual library functions
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib") //for shader loading function



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
		nullptr,			//choose default adapter
		D3D_DRIVER_TYPE_HARDWARE, //Use hardware driver 
		nullptr,			//
		swapCreateFlags,	// Create device on debug layer so we get debug info in VS error log
		nullptr,			//no feature levels specified GPU needs to support
		0,					//
		D3D11_SDK_VERSION,	//sdk version targetting (which D3D needs to be installed
		&sd,				//descriptor structure for configuring swap chain
		pSwap.ReleaseAndGetAddressOf(),		//fills in ptr to swap chain
		pDevice.ReleaseAndGetAddressOf(),	//fills in ptr to device
		nullptr,							//
		pContext.ReleaseAndGetAddressOf()	//fills in ptr to context
	) );

	//Gain access to texture subresource in swap chain (backbuffer)
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer; 
	GFX_THROW_INFO( pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer) );
	GFX_THROW_INFO( pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget) );	

	//Create and bind depth buffer state
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS; // if z-value is less then it's closer so draw on top of render buffer
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO( pDevice->CreateDepthStencilState(&dsDesc, &pDSState) );
	//bind depth buffer state to output merger
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	//Create and bind depth buffer texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;	//Match width/height to swap chain dimensions
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u; //Single texture stored (it's possible to store an array of textures)
	descDepth.Format = DXGI_FORMAT_D32_FLOAT; //special DXGI format for "Depth 32-bit float" = D32
	descDepth.SampleDesc.Count = 1u; //for anti-aliasing
	descDepth.SampleDesc.Quality = 0u; //also for anti-aliasing
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL; //specify binding for depth buffer
	GFX_THROW_INFO( pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil) );	//Empty texture data (depth info is generated per frame)
	
	//Need to get a view on the depth texture to bind it to the pipeline, but this should be maintained as graphics class member
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;	
	GFX_THROW_INFO( pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV) );

	//Bind depth stencil view to output merger
	pContext->OMSetRenderTargets( 1u, pTarget.GetAddressOf(), pDSV.Get() );


	//Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vp);

	//Init imgui d3d impl
	ImGui_ImplDX11_Init( pDevice.Get(), pContext.Get() );
}


void Graphics::EndFrame()
{
	//imgui frame end
	if (imguiEnabled)
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
	}

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

void Graphics::BeginFrame(float red, float green, float blue) noexcept
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	const float colour[] = { red, green, blue, 1.0f };
	//Clear render target (backbuffer) into an RGB colour using Context which controls rendering
	pContext->ClearRenderTargetView(pTarget.Get(), colour);
	//Also need to clear depth buffer every frame
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawIndexed(UINT count) noexcept (!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(
		pContext->DrawIndexed(count, 0u, 0u)
	);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;

}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

void Graphics::EnableImgui() noexcept
{
	imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
	imguiEnabled = false;
}

bool Graphics::IsImGuiEnabled() const noexcept
{
	return imguiEnabled;
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

Graphics::InfoException::InfoException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception{ line, file }
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

const char * Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char * Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}
