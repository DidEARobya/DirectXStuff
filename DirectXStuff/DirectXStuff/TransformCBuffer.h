#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformCBuffer : public Bindable
{
public:
	TransformCBuffer(Graphics& graphics, const Drawable& parent, UINT slot = 0);
	void Bind(Graphics& graphics) noexcept override;

private:
	struct Transforms
	{
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX modelViewProj;
	};

	static std::unique_ptr<VertexConstantBuffer<Transforms>> _pvConstantBuffer;
	const Drawable& _parent;
};

