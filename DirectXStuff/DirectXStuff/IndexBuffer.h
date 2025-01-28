#pragma once
#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(Graphics& graphics, const std::vector<unsigned short>& indices);

	void Bind(Graphics& graphics) noexcept override;
	UINT GetCount() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBuffer;
	UINT _count;
};

