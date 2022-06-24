#pragma once
#include "Graphics.h"

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
private:
	float r = 20.0f; //Distance from origin
	float theta = 0.0f; //Rotation around origin
	float phi = 0.0f; //Rotation around origin
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
};

