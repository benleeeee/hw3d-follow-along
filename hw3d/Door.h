#pragma once
#include "DrawableBase.h"

///
/// This class was made as a test to see if I could replicate a drawable object
/// in my own class. I tried to 'cheat' as little as possible but had to check
/// quite a lot still
/// 


//Template on Door so it has it's own door-specific static bindables
class Door : public DrawableBase<Door>
{
public:
	Door( Graphics& gfx, std::mt19937& rng,				//Need to pass gfx to almost everything
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist ); 
	//<--- Inherits virtual destructor from Drawable
	void Update( float dt ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
	// positional
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;
	// speed (delta/s)
	float droll;
	float dpitch;
	float dyaw;
	float dtheta;
	float dphi;
	float dchi;
	DirectX::XMFLOAT3X3 mt;
};