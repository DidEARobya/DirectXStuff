#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	bool IsStaticInitialised() const noexcept
	{
		return !_staticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
		_staticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept (!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && _pIndexBuffer == nullptr);
		_pIndexBuffer = indexBuffer.get();
		_staticBinds.push_back(std::move(indexBuffer));
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && _pIndexBuffer == nullptr);

		for (const auto& b : _staticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				_pIndexBuffer = p;
				return;
			}
		}

		assert("Failed to find index buffer in static binds" && _pIndexBuffer != nullptr);
	}
private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return _staticBinds;
	}

private:
	static std::vector<std::unique_ptr<Bindable>> _staticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::_staticBinds;
