#include "SolidSphere.h"
#include "BindableIncludes.h"
#include "GraphicsMacros.h"
#include "Sphere.h"


SolidSphere::SolidSphere(Graphics& graphics, float radius)
{
	namespace dirx = DirectX;

	if (!IsStaticInitialised())
	{
		struct Vertex
		{
			dirx::XMFLOAT3 pos;
		};

		auto model = Sphere::Make<Vertex>();
		model.Transform(dirx::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<VertexBuffer>(graphics, model._vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(graphics, model._indices));

		auto pvs = std::make_unique<VertexShader>(graphics, L"SolidVS.cso");
		auto pvsbc = pvs->GetByteCode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"SolidPS.cso"));

		struct PSColorConstant
		{
			dirx::XMFLOAT3 colour = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(graphics, colorConst));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
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

void SolidSphere::Update(float dt) noexcept {}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	_pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformMatrix() const noexcept
{
	return DirectX::XMMatrixTranslation(_pos.x, _pos.y, _pos.z);
}