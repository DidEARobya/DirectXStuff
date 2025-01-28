#include "Drawable.h"
#include "GraphicsMacros.h"
#include "IndexBuffer.h"
#include <cassert>
#include <typeinfo>

void Drawable::Draw(Graphics& graphics) const noexcept(!IS_DEBUG)
{
	for (auto& b : _binds)
	{
		b->Bind(graphics);
	}
	for (auto& b : GetStaticBinds())
	{
		b->Bind(graphics);
	}

	graphics.DrawIndexed(_pIndexBuffer->GetCount());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept (!IS_DEBUG)
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	_binds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> indexBuffer) noexcept (!IS_DEBUG)
{
	assert("Attempting to add infex buffer a second time" && _pIndexBuffer == nullptr);
	_pIndexBuffer = indexBuffer.get();
	_binds.push_back(std::move(indexBuffer));
}
