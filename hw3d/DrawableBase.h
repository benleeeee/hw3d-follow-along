#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialised() noexcept
	{
		return !staticBinds.empty(); //assumes not initialised if vector is empty
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> ibuf) noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);
		pIndexBuffer = ibuf.get();
		staticBinds.push_back(std::move(ibuf));
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && pIndexBuffer == nullptr);

		//Static binds are only set up once which leads to AddStaticIndexBuffer only running for first drawable
		//However this func doubles up as setting the ptr to index buffer, so this func is necessary to loop
		//through static binds and find the index buffer to set as ptr in drawable class
		for (const auto& b : staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				pIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return staticBinds;
	}
private:
	//Static base vector so each drawable class that inherits templates and gets their own unique static variable
	static std::vector<std::unique_ptr<Bindable>> staticBinds;
};


//Need to define the variable outside the class after declaring in class
template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;