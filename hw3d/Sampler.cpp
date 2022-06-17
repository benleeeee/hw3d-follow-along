#include "Sampler.h"
#include "GraphicsThrowMacros.h"

Sampler::Sampler( Graphics& gfx )
{
	INFOMAN( gfx );


	D3D11_SAMPLER_DESC desc = {};
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	
	GFX_THROW_INFO( 
		GetDevice( gfx )->CreateSamplerState( &desc, &pSampler ) 
	);
}

void Sampler::Bind( Graphics& gfx ) noexcept
{
	GetContext( gfx )->PSSetSamplers( 0, 1, pSampler.GetAddressOf() );
}
