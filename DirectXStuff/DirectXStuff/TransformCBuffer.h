#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformCBuffer : public Bindable
{
public:
	TransformCBuffer(Graphics& graphics, const Drawable& parent);
	void Bind(Graphics& graphics) noexcept override;

private:
	static std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> _pvConstantBuffer;
	const Drawable& _parent;
};

