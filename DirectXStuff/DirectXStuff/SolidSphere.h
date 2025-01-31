#pragma once
#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(Graphics& graphics, float radius);

	void Update(float deltaTime) noexcept override;
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 _pos = { 1.0f,1.0f,1.0f };
};

