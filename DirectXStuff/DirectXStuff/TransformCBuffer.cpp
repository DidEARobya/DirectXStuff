#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer(Graphics& graphics, const Drawable& parent, UINT slot) : _parent(parent)
{
	if (_pvConstantBuffer == nullptr)
	{
		_pvConstantBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(graphics, slot);
	}
}

void TransformCBuffer::Bind(Graphics& graphics) noexcept
{
	const auto modelView = _parent.GetTransformMatrix() * graphics.GetCamera();

	const Transforms transforms =
	{
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(modelView * graphics.GetProjection())
	};

	_pvConstantBuffer->Update(graphics, transforms);
	_pvConstantBuffer->Bind(graphics);
}

std::unique_ptr<VertexConstantBuffer<TransformCBuffer::Transforms>> TransformCBuffer::_pvConstantBuffer;