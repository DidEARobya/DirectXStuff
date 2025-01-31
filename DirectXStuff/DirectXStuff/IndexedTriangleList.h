#pragma once
#include <vector>
#include <DirectXMath.h>

template<class T>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<T> verts_in, std::vector<unsigned short> indices_in) : _vertices(std::move(verts_in)), _indices(std::move(indices_in))
	{
		assert(_vertices.size() > 2);
		assert((_indices.size() % 3) == 0);
	}

	void Transform(DirectX::FXMMATRIX matrix)
	{
		for (auto& v : _vertices)
		{
			const DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&v.pos);
			DirectX::XMStoreFloat3(&v.pos, DirectX::XMVector3Transform(pos, matrix));
		}
	}

	void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		assert(_indices.size() % 3 == 0 && _indices.size() > 0);

		for (size_t i = 0; i < _indices.size(); i += 3)
		{
			auto& v0 = _vertices[_indices[i]];
			auto& v1 = _vertices[_indices[i + 1]];
			auto& v2 = _vertices[_indices[i + 2]];

			const XMVECTOR p0 = XMLoadFloat3(&v0.pos);
			const XMVECTOR p1 = XMLoadFloat3(&v1.pos);
			const XMVECTOR p2 = XMLoadFloat3(&v2.pos);

			const XMVECTOR n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			XMStoreFloat3(&v0.n, n);
			XMStoreFloat3(&v1.n, n);
			XMStoreFloat3(&v2.n, n);
		}
	}
public:
	std::vector<T> _vertices;
	std::vector<unsigned short> _indices;
};

