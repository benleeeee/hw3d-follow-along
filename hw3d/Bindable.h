#pragma once
#include "Graphics.h"

//Interface class for objects bindable to pipeline
class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	//Access functions to get some internal members of Graphics that are private.
	//The base 'Bindable' class is a friend class to Graphics so has access, but all those
	//that inherit from this interface don't have direct access, so must use these accessors.
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept(!IS_DEBUG);
};

