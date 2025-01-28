#include "Melon.h"
#include "BindableIncludes.h"
#include "GraphicsMacros.h"
#include "Sphere.h"

namespace dirx = DirectX;

Melon::Melon(Graphics& graphics, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_int_distribution<int>& longdist, std::uniform_int_distribution<int>& latdist)
	: 
	_r(rdist(rng)),_droll(ddist(rng)),_dpitch(ddist(rng)),_dyaw(ddist(rng)),_dphi(odist(rng)),_dtheta(odist(rng)),_dchi(odist(rng)),_chi(adist(rng)),_theta(adist(rng)),_phi(adist(rng))
{
	if (IsStaticInitilised() == false)
	{
		auto pVertexShader = std::make_unique<VertexShader>(graphics, L"ColourIndexVS.cso");
		auto pvsbc = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"ColourIndexPS.cso"));

		struct PixelShaderConstants
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colours[8];
		};
		const PixelShaderConstants pixelShaderConstants =
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(graphics, pixelShaderConstants));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pvsbc));

		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	struct Vertex
	{
		dirx::XMFLOAT3 pos;
	};

	auto model = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));
	// deform vertices of model by linear transformation
	model.Transform(dirx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

	AddBind(std::make_unique<VertexBuffer>(graphics, model._vertices));

	AddIndexBuffer(std::make_unique<IndexBuffer>(graphics, model._indices));

	AddBind(std::make_unique<TransformCBuffer>(graphics, *this));
}

void Melon::Update(float deltaTime) noexcept
{
	_roll += _droll * deltaTime;
	_pitch += _dpitch * deltaTime;
	_yaw += _dyaw * deltaTime;
	_theta += _dtheta * deltaTime;
	_phi += _dphi * deltaTime;
	_chi += _dchi * deltaTime;
}

DirectX::XMMATRIX Melon::GetTransformMatrix() const noexcept
{
	return dirx::XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll) *
		dirx::XMMatrixTranslation(_r, 0.0f, 0.0f) *
		dirx::XMMatrixRotationRollPitchYaw(_theta, _phi, _chi) *
		dirx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}
