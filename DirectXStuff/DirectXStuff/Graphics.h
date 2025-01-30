#pragma once
#include "WinInclude.h"
#include "WinException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "DXGIInfoManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include <string>

class Graphics
{
	friend class Bindable;
public:
	class Exception : public WinException
	{
		using WinException::WinException;
	};
	class GraphicsException : public Exception
	{
	public:
		GraphicsException(int line, const char* file, HRESULT hResult, std::vector<std::string> exceptionInfo = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		static std::string TranslateErrorCode(HRESULT hResult) noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;

	private:
		HRESULT _hResult;
		std::string _info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> info) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string _info;
	};
	class DeviceRemovedException : public GraphicsException
	{
		using GraphicsException::GraphicsException;

	public:
		const char* GetType() const noexcept override;
	};
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator= (const Graphics&) = delete;
	~Graphics();

	void BeginFrame(float r, float g, float b) noexcept;
	void EndFrame();
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX projection) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;

	void SetCamera(DirectX::FXMMATRIX camera) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;	
private:
	DirectX::XMMATRIX _projection;
	DirectX::XMMATRIX _camera;

	bool _isImGuiEnabled = true;

#if defined(_DEBUG)
	DXGIInfoManager _infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> _pDevice = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pTarget = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _pDepthView = nullptr;
};
