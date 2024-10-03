#include "Graphics.h"
#include <sstream>
#include "iGManager.h"

#include "Exception/ExceptionMacros.h"

//	Graphics stuff

Graphics::~Graphics()
{
}

Graphics::Graphics(HWND hWnd)
{
	create(hWnd);
}

void Graphics::create(HWND hWnd)
{
	HWnd = hWnd;
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width						= 0u;
	sd.BufferDesc.Height					= 0u;
	sd.BufferDesc.Format					= DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator		= 0u;
	sd.BufferDesc.RefreshRate.Denominator	= 0u;
	sd.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count						= 1u;
	sd.SampleDesc.Quality					= 0u;
	sd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount							= 1u;
	sd.OutputWindow							= hWnd;
	sd.Windowed								= TRUE;
	sd.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags								= 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//	Create device & front/back buffers & swap chain & rendering context

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	//	Gain access to render target through shinnanigins

	pCom<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

	//	Create and bind Perspective constant buffer

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(_float4matrix) + sizeof(_float4vector);
	cbd.StructureByteStride = 0u;
	GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, NULL, &pPerspective));

	GFX_THROW_INFO_ONLY(pContext->VSSetConstantBuffers(0u, 1u, pPerspective.GetAddressOf()));

	//	Initialize imGui

	iGManager::initDX11(pDevice.Get(), pContext.Get());
}

bool Graphics::isInitialized()
{
	if (pDevice.Get())
		return true;
	return false;
}

void Graphics::pushFrame()
{
	HRESULT hr;

	if (FAILED(hr = pSwap->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		else
			throw GFX_EXCEPT(hr);
	}
}

void Graphics::clearBuffer(float R, float G, float B, float A)
{
	const float color[] = { R,G,B,A };
	GFX_THROW_INFO_ONLY(pContext->ClearRenderTargetView(pTarget.Get(), color));
	GFX_THROW_INFO_ONLY(pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u));
}

void Graphics::clearBuffer(Color color)
{
	_float4color col = color.getColor4();
	GFX_THROW_INFO_ONLY(pContext->ClearRenderTargetView(pTarget.Get(), &col.r));
	GFX_THROW_INFO_ONLY(pContext->ClearDepthStencilView(pDSV.Get(),D3D11_CLEAR_DEPTH,1.f,0u));
}

void Graphics::clearDepthBuffer()
{
	GFX_THROW_INFO_ONLY(pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u));
}

Vector2f Graphics::PixeltoR2(Vector2i MousePos)
{
	return Vector2f(2.f * MousePos.x / WindowDim.x - 1.f, -2.f * MousePos.y / WindowDim.y + 1.f);
}

void Graphics::setWindowDimensions(Vector2i& Dim)
{
	WindowDim = Dim;

	if (!WindowDim.x && !WindowDim.y)
		return;

	GFX_THROW_INFO_ONLY(pContext->OMSetRenderTargets(0u, NULL, NULL));

	pTarget->Release();

	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.

	GFX_THROW_INFO(pSwap->ResizeBuffers(0u, 0u, 0u, DXGI_FORMAT_UNKNOWN, 0u));

	// Get buffer and create a render-target-view.

	pCom<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, pTarget.GetAddressOf()));

	//	Create and bind depth stencil state

	pCom<ID3D11DepthStencilState> pDSState;
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	GFX_THROW_INFO_ONLY(pContext->OMSetDepthStencilState(pDSState.Get(), 1u));

	//	Create and bind depth stencil texture

	pCom<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = (UINT)WindowDim.x;
	descDepth.Height = (UINT)WindowDim.y;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&descDepth, NULL, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV));

	//	Bind render target

	GFX_THROW_INFO_ONLY(pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get()));

	CD3D11_VIEWPORT vp;
	vp.Width = (float)Dim.x;
	vp.Height = (float)Dim.y;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	GFX_THROW_INFO_ONLY(pContext->RSSetViewports(1u, &vp));

	//	Update perspective to match scaling

	Matrix Projections = ProjectionMatrix(Observer) * ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * Scale;
	cbuff.perspective = Projections.transpose().getMatrix4();
	cbuff.traslation = Center.getVector4();

	GFX_THROW_INFO_ONLY(pContext->UpdateSubresource(pPerspective.Get(), 0u, NULL, &cbuff, 0u, 0u));

	clearDepthBuffer();
}

Vector2i Graphics::getWindowDimensions()
{
	return WindowDim;
}

void Graphics::updatePerspective(Vector3f obs, Vector3f center, float scale)
{
	if (!obs)
		throw std::exception("The observer must be a vector diferent than zero");

	Observer = obs.normalize();
	Center = center;
	Scale = scale;

	if (Observer == Vector3f(0.f, 0.f, -1.f))
	{
		Matrix Projections = ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * scale;
		cbuff.perspective = Projections.transpose().getMatrix4();
	}

	else
	{
		Matrix Projections = ProjectionMatrix(obs) * ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * scale;
		cbuff.perspective = Projections.transpose().getMatrix4();
	}


	cbuff.traslation = center.getVector4();

	GFX_THROW_INFO_ONLY(pContext->UpdateSubresource(pPerspective.Get(), 0u, NULL, &cbuff, 0u, 0u));
}

void Graphics::drawIndexed(UINT IndexCount)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(IndexCount, 0u, 0u));
}

Vector3f Graphics::getObserver()
{
	return Observer;
}

Vector3f Graphics::getCenter()
{
	return Center;
}

float Graphics::getScale()
{
	return Scale;
}
