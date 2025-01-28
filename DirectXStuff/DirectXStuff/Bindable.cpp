#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(Graphics& graphics) noexcept
{
	return graphics._pContext.Get();
}

ID3D11Device* Bindable::GetDevice(Graphics& graphics) noexcept
{
	return graphics._pDevice.Get();
}

DXGIInfoManager& Bindable::GetInfoManager(Graphics& graphics) noexcept(IS_DEBUG)
{
#if defined (_DEBUG)
	return graphics._infoManager;
#else
	throw std::logic_error("Tried to access graphics._infoManager in Release config)");
#endif
}