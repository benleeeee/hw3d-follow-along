#pragma once
#include "Bindable.h"

class Sampler : public Bindable
{
public:
	Sampler( Graphics& gfx );
	void Bind( Graphics& gfx ) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler; //Tells the pipeline how to perform lookups into a sampled texture
};

