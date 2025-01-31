#include "Cylinder.h"
#include "BindableIncludes.h"
#include "GraphicsMacros.h"
#include "Prism.h"

Cylinder::Cylinder(Graphics& graphics, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, std::uniform_int_distribution<int>& tdist)
	:
	BaseObject(graphics, rng, adist, ddist, odist, rdist)
{
	namespace dirx = DirectX;

	if (IsStaticInitialised() == false)
	{
		struct Vertex
		{
			dirx::XMFLOAT3 pos;
			dirx::XMFLOAT3 n;
		};

		auto model = Prism::MakeTesselatedIndependentCapNormals<Vertex>(tdist(rng));	

		AddStaticBind(std::make_unique<VertexBuffer>(graphics, model._vertices));

		auto pVertexShader = std::make_unique<VertexShader>(graphics, L"PhongVS.cso");
		auto pVSByteCode = pVertexShader->GetByteCode();
		AddStaticBind(std::move(pVertexShader));

		AddStaticBind(std::make_unique<PixelShader>(graphics, L"IndexedPhongPS.cso"));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(graphics, model._indices));

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};

		AddStaticBind(std::make_unique<InputLayout>(graphics, ied, pVSByteCode));
		AddStaticBind(std::make_unique<Topology>(graphics, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			 dirx::XMFLOAT3A colours[6] =
			{
				{1.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{0.0f, 0.0f, 1.0f},
				{1.0f, 1.0f, 0.0f},
				{1.0f, 0.0f, 1.0f},
				{0.0f, 1.0f, 1.0f},
			};

			float specularIntensity = 0.6f;
			float specularPower = 30.0f;

		}materialConst;

		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(graphics, materialConst, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCBuffer>(graphics, *this));
}
