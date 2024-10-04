#pragma once

#include "WinHeader.h"
#include "Header.h"

#include "Exception/Exception.h"
#include "Exception/DxgiInfoManager.h"

#include <iostream>
#include <vector>
#include <queue>

#define pCom Microsoft::WRL::ComPtr

enum GPU_PREFERENCE
{
	GPU_HIGH_PERFORMANCE,
	GPU_MINIMUM_POWER,
	GPU_UNSPECIFIED,
};

class Graphics
{
	friend class Bindable;
public:
	class HrException : public ExceptionClass
	{
	public:
		HrException(int line, const char* file, HRESULT hr, const char** infoMsgs = nullptr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public ExceptionClass
	{
	public:
		InfoException(int line, const char* file, const char** infoMsgs = nullptr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};

public:
	Graphics() {}
	~Graphics();
	Graphics(HWND hWnd, GPU_PREFERENCE preference);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void create(HWND hWnd, GPU_PREFERENCE preference);
	bool isInitialized();

	void pushFrame();
	void clearBuffer(float R = 1.f, float G = 1.f, float B = 1.f, float A = 1.f);
	void clearBuffer(Color color);
	void clearDepthBuffer();

	Vector2f PixeltoR2(Vector2i MousePos);
	void setWindowDimensions(Vector2i& Dim);
	Vector2i getWindowDimensions();
	void updatePerspective(Vector3f obs, Vector3f center, float scale);
	void drawIndexed(UINT IndexCount);

	Vector3f getObserver();
	Vector3f getCenter();
	float getScale();

private:
	HWND HWnd;

#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	pCom<ID3D11Device>				pDevice;
	pCom<IDXGISwapChain>			pSwap;
	pCom<ID3D11DeviceContext>		pContext;
	pCom<ID3D11RenderTargetView>	pTarget;
	pCom<ID3D11DepthStencilView>	pDSV;
	pCom<ID3D11Buffer>				pPerspective;

	struct {
		_float4matrix perspective;
		_float4vector traslation;
	}cbuff;

	Vector2i WindowDim;
	Vector3f Observer = { 0.f , -1.f , 0.f };
	Vector3f Center = { 0.f , 0.f , 0.f };
	float Scale = 250.f;
};
