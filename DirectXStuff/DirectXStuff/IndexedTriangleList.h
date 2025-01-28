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
public:
	std::vector<T> _vertices;
	std::vector<unsigned short> _indices;
};

