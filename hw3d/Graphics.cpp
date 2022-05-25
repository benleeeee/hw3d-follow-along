#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

//Set the linker settings for d3d11 library, necessary to link to actual library functions
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib") //for shader loading function

//### Macros for easier typing to throw exceptions ###
// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
//creates specialised deviceRemovedException, which can be thrown manually
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
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
	pContext->ClearRenderTargetView(pTarget.Get(), colour);
}

void Graphics::DrawTestTriangle()
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex
	{
		struct {
			float x;
			float y;
		} pos;
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		} colour;
	};
	Vertex vertices[] =
	{
		{ 0.0f, 0.5f, 255, 0, 0, 0 },
		{ 0.5f, -0.5f, 0, 255, 0, 0 },
		{ -0.5f, -0.5f, 0, 0, 255, 0 },
		{ -0.3f, 0.3f, 0, 255, 0, 0 },
		{ 0.3f, 0.3f, 0,0, 255, 0 },
		{ 0.0f, -0.8f, 255, 0, 0, 0 },
	};
	const unsigned short indices[] =
	{
		0,1,2,
		0,2,3,
		0,4,1,
		2,1,5
	};

	//Create buffer description descriptor
	D3D11_BUFFER_DESC desc = D3D11_BUFFER_DESC();
	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0u;
	desc.MiscFlags = 0u;
	desc.ByteWidth = sizeof(vertices);
	desc.StructureByteStride = sizeof(Vertex);


	//Create initial data
	D3D11_SUBRESOURCE_DATA data = D3D11_SUBRESOURCE_DATA();
	data.pSysMem = vertices; //does this not need &?
	

	//Create Vertex Buffer ComPtr	
	wrl::ComPtr<ID3D11Buffer> pVBuff;
	//Create Buffer ComObj and fill pVBuff w/ it
	GFX_THROW_INFO(pDevice->CreateBuffer(&desc, &data, &pVBuff));
	//Needed to configure binding of VBuff to pipeline below
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	//Bind buffer to pipeline
	pContext->IASetVertexBuffers(0u, 1u, pVBuff.GetAddressOf(), &stride, &offset);


	//Repurpose values in desc and data descriptors for Index Buffer
	desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(indices);
	desc.CPUAccessFlags = 0u;
	desc.MiscFlags = 0u;
	desc.StructureByteStride = sizeof(unsigned short);
	desc.Usage = D3D11_USAGE_DEFAULT;
	data.pSysMem = indices; //Set data to index buffer content

	//Create Indices Buffer ComPtr	
	wrl::ComPtr<ID3D11Buffer> pIBuff;
	GFX_THROW_INFO(pDevice->CreateBuffer(&desc, &data, &pIBuff));	
	pContext->IASetIndexBuffer(pIBuff.Get(), DXGI_FORMAT_R16_UINT, 0u);


	//Create pixel shader comobj
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob; //bytecode file comObj
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob)); //simultaneously calls pBlob->release() in & opeartor
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	//Bind pixel shader to pipeline
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	//Bind render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	//Create vertex shader comobj
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	//Fill pBlob ptr with binaries read from Vertex Shader compiled file
	GFX_THROW_INFO( D3DReadFileToBlob(L"VertexShader.cso", &pBlob) ); 
	GFX_THROW_INFO( pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader) );	
	//Bind vertex shader to pipeline
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);		

	//Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 600;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	//Set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create input layout object to define how our vertex struct interacts w/ pipeline
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	//Use array of descriptors to allow multiple descriptions
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{		
		{"Position",			//Semantic name
		0,						//Semantic index
		DXGI_FORMAT_R32G32_FLOAT, //Format (two 32bit floats for x,y)
		0,						//Input slot
		0,						//Aligned byte offset
		D3D11_INPUT_PER_VERTEX_DATA, //Input slot class
		0 },					//Instance data step rate
		{"Colour", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};	
	GFX_THROW_INFO( pDevice->CreateInputLayout(
		ied,
		(UINT)std::size(ied),
		pBlob->GetBufferPointer(),	//func wants shader bytecode for vertex shader so that
		pBlob->GetBufferSize(),		//it can check input element desc against shader code
		&pInputLayout)
	);
	//Bind inputLayout to pipeline
	pContext->IASetInputLayout(pInputLayout.Get());

	//DRAW!!!
	GFX_THROW_INFO_ONLY( 
		pContext->DrawIndexed((UINT)std::size(indices), 0u, 0) 
	);
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
