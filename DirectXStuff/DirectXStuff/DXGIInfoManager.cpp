#include "DXGIInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <memory>

#pragma comment(lib, "dxguid.lib")

DXGIInfoManager::DXGIInfoManager()
{
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const auto hModDXGIDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDXGIDebug == nullptr)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hModDXGIDebug, "DXGIGetDebugInterface")));

	if (DxgiGetDebugInterface == nullptr)
	{
		throw WINDOW_LAST_EXCEPT();
	}

	HRESULT hResult;
	GRAPHICS_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &_pDXGIInfoQueue));
}

void DXGIInfoManager::Set() noexcept
{
	//Makes sure that the next GetMessages() call only gets messages that occured after this was called last.
	_next = _pDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DXGIInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = _pDXGIInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

	for (auto i = _next; i < end; i++)
	{
		HRESULT hResult;
		SIZE_T messageLength;
		// Get the size of message i in bytes
		GRAPHICS_THROW_NOINFO(_pDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// Allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// Get the message and push its description into the vector
		GRAPHICS_THROW_NOINFO(_pDXGIInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}

	return messages;
}
