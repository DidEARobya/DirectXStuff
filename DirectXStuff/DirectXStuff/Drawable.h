#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual ~Drawable() = default;

	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;
	virtual void Update(float deltaTime) noexcept = 0;

	void Draw(Graphics& graphics) const noexcept(!IS_DEBUG);

protected:
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> indexBuffer) noexcept(!IS_DEBUG);

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const class IndexBuffer* _pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> _binds;
};

