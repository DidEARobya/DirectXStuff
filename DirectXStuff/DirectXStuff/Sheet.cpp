#include "Sheet.h"
#include "BindableIncludes.h"
#include "GraphicsMacros.h"
#include "Plane.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"

namespace dirx = DirectX;

Sheet::Sheet(Graphics& graphics,std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	:
	_r(rdist(rng)),_droll(ddist(rng)),_dpitch(ddist(rng)),_dyaw(ddist(rng)),_dphi(odist(rng)),_dtheta(odist(rng)),_dchi(odist(rng)),_chi(adist(rng)),_theta(adist(rng)),_phi(adist(rng))
{
	if (IsStaticInitialised() == false)
	{
		struct Vertex
		{
			dirx::XMFLOAT3 pos;
			struct
			{
				float u;
				float v;
			} tex;
		};
		auto model = Plane::Make<Vertex>();
		model._vertices[0].tex = { 0.0f,0.0f };
		model._vertices[1].tex = { 1.0f,0.0f };
		model._vertices[2].tex = { 0.0f,1.0f };
		model._vertices[3].tex = { 1.0f,1.0f };

		AddStaticBind(std::make_unique<Texture>(graphics, Surface::FromFile("Images\\kappa50.png")));

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model._vertices));

		AddStaticBind(std::make_unique<Sampler>(graphics));

		auto pVertexShader = std::make_unique<VertexShader>(graphics, L"TextureVS.cso");
		auto pVSByteCode = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"TexturePS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pVSByteCode));

		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuffer>(graphics, *this));
}

void Sheet::Update(float deltaTime) noexcept
{
	_roll += _droll * deltaTime;
	_pitch += _dpitch * deltaTime;
	_yaw += _dyaw * deltaTime;
	_theta += _dtheta * deltaTime;
	_phi += _dphi * deltaTime;
	_chi += _dchi * deltaTime;
}

DirectX::XMMATRIX Sheet::GetTransformMatrix() const noexcept
{
	return dirx::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
		dirx::XMMatrixTranslation(_r, 0.0f, 0.0f) *
		dirx::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi) *
		dirx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}