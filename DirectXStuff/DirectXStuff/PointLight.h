#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffer.h"

class PointLight
{
public:
	PointLight(Graphics& graphics, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(Graphics& graphics) const noexcept(!IS_DEBUG);
	void Bind(Graphics& graphics, DirectX::FXMMATRIX view) const noexcept;
private:
	struct PointLightCBuffer
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColour;
		float diffuseIntensity;
		float attenuationConst;
		float attenuationLin;
		float attenuationQuad;
	};
private:
	PointLightCBuffer _cBufferData;
	mutable SolidSphere _mesh;
	mutable PixelConstantBuffer<PointLightCBuffer> _cBuffer;
};

