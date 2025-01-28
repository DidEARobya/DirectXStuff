#include "Pyramid.h"
#include "BindableIncludes.h"
#include "GraphicsMacros.h"
#include "Cone.h"

namespace dirx = DirectX;

Pyramid::Pyramid(Graphics& graphics,std::mt19937& rng,std::uniform_real_distribution<float>& adist,std::uniform_real_distribution<float>& ddist,std::uniform_real_distribution<float>& odist,std::uniform_real_distribution<float>& rdist)
	:
	_r(rdist(rng)),_droll(ddist(rng)),_dpitch(ddist(rng)),_dyaw(ddist(rng)),_dphi(odist(rng)),_dtheta(odist(rng)),_dchi(odist(rng)),_chi(adist(rng)),_theta(adist(rng)),_phi(adist(rng))
{
	if (IsStaticInitilised() == false)
	{
		struct Vertex
		{
			dirx::XMFLOAT3 pos;
			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} colour;
		};
		auto model = Cone::MakeTesselated<Vertex>(4);
		// set vertex colours for mesh
		model._vertices[0].colour = { 255,255,0 };
		model._vertices[1].colour = { 255,255,0 };
		model._vertices[2].colour = { 255,255,0 };
		model._vertices[3].colour = { 255,255,0 };
		model._vertices[4].colour = { 255,255,80 };
		model._vertices[5].colour = { 255,10,0 };

		// deform mesh linearly
		model.Transform(dirx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model._vertices));

		auto pVertexShader = std::make_unique<VertexShader>(graphics, L"ColourBlendVS.cso");
		auto pvsbc = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"ColourBlendPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Colour",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuffer>(graphics, *this));
}

void Pyramid::Update(float deltaTime) noexcept
{
	_roll += _droll * deltaTime;
	_pitch += _dpitch * deltaTime;
	_yaw += _dyaw * deltaTime;
	_theta += _dtheta * deltaTime;
	_phi += _dphi * deltaTime;
	_chi += _dchi * deltaTime;
}

DirectX::XMMATRIX Pyramid::GetTransformMatrix() const noexcept
{
	return dirx::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
		dirx::XMMatrixTranslation(_r, 0.0f, 0.0f) *
		dirx::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi);
}
