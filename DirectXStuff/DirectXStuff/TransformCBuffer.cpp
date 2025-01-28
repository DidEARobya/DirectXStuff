#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer(Graphics& graphics, const Drawable& parent) : _parent(parent)
{
	if (_pvConstantBuffer == nullptr)
	{
		_pvConstantBuffer = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(graphics);
	}
}

void TransformCBuffer::Bind(Graphics& graphics) noexcept
{
	_pvConstantBuffer->Update(graphics, DirectX::XMMatrixTranspose(_parent.GetTransformMatrix() * graphics.GetProjection()));
	_pvConstantBuffer->Bind(graphics);
}

std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCBuffer::_pvConstantBuffer;