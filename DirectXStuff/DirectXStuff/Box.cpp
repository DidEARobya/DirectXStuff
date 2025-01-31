#include "Box.h"
#include "BindableIncludes.h"
#include "GraphicsMacros.h"
#include "Cube.h"

namespace dirx = DirectX;

Box::Box(Graphics& graphics, std::mt19937& rng,std::uniform_real_distribution<float>& adist,std::uniform_real_distribution<float>& ddist,std::uniform_real_distribution<float>& odist,std::uniform_real_distribution<float>& rdist,std::uniform_real_distribution<float>& bdist, DirectX::XMFLOAT3 material)
	: 
	BaseObject(graphics, rng, adist, ddist, odist, rdist)
{
	if (IsStaticInitialised() == false)
	{
		struct Vertex
		{
			dirx::XMFLOAT3 pos;
			dirx::XMFLOAT3 n;
		};

		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model._vertices));

		auto pVertexShader = std::make_unique<VertexShader>(graphics, L"PhongVS.cso");
		auto pVSByteCode = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"PhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pVSByteCode));
		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuffer>(graphics, *this));

	struct PSMaterialConstant
	{
		 dirx::XMFLOAT3 colour;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];

	}colourConst;

	colourConst.colour = material;
	AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphics, colourConst, 1u));

	dirx::XMStoreFloat3x3(&_modelTransform, dirx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

DirectX::XMMATRIX Box::GetTransformMatrix() const noexcept
{
	namespace dirx = DirectX;
	return dirx::XMLoadFloat3x3(&_modelTransform) * BaseObject::GetTransformMatrix();
}