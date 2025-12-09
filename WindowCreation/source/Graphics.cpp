#include "Graphics.h"
#include "Exception/_exGraphics.h"

#include "WinHeader.h"
#include <dxgi1_6.h> // For GPU selection

/*
--------------------------------------------------------------------------------------------
 Global Device Functions
--------------------------------------------------------------------------------------------
*/

// This struct contains the data for the global device.
struct DEVICE_DATA
{
	ComPtr<ID3D11Device>		pDevice;
	ComPtr<ID3D11DeviceContext>	pContext;
};

// Helper to delete the global device at the end of the process.
GlobalDevice GlobalDevice::helper;
GlobalDevice::~GlobalDevice()
{
	if (globalDeviceData)
		delete (DEVICE_DATA*)globalDeviceData;
}

void* GlobalDevice::globalDeviceData = nullptr; // Stores the device data masked as void*

// Sets the global devica according to the GPU preference, must be set before
// creating any window instance, otherwise it will be ignored.
// If none is set it will automatically be created by the first window creation.

void GlobalDevice::set_global_device(GPU_PREFERENCE preference)
{
	if (globalDeviceData)
		return;

	globalDeviceData = new DEVICE_DATA;
	DEVICE_DATA& data = *(DEVICE_DATA*)globalDeviceData;

	//  Create Factory

	ComPtr<IDXGIFactory> dxgiFactory;
	GFX_THROW_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), &dxgiFactory));

	//  Find adapter by GPU preference

	ComPtr<IDXGIAdapter> bestAdapter = nullptr;

	ComPtr<IDXGIFactory6> Factory6;
	GFX_THROW_INFO(dxgiFactory.As(&Factory6));

	switch (preference)
	{
	case GPU_HIGH_PERFORMANCE:
		GFX_THROW_INFO(Factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&bestAdapter)));
		break;
	case GPU_MINIMUM_POWER:
		GFX_THROW_INFO(Factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_MINIMUM_POWER, IID_PPV_ARGS(&bestAdapter)));
		break;
	case GPU_UNSPECIFIED:
		GFX_THROW_INFO(Factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&bestAdapter)));
		break;
	}

	//  Create D3D11 device and context with the chosen adapter

	GFX_THROW_INFO(D3D11CreateDevice(
		bestAdapter.Get(),
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		0u,
		nullptr,
		0u,
		D3D11_SDK_VERSION,
		&data.pDevice,
		nullptr,
		&data.pContext
	));
}

// Internal function that returns the ID3D11Device* masked as a void*.

void* GlobalDevice::get_device_ptr()
{
	DEVICE_DATA& data = *(DEVICE_DATA*)globalDeviceData;

	return data.pDevice.Get();
}

// Internal function that returns the ID3D11DeviceContext* masked as void*
void* GlobalDevice::get_context_ptr()
{
	DEVICE_DATA& data = *(DEVICE_DATA*)globalDeviceData;

	return data.pContext.Get();
}

// Mini helpers to make the code more readable

#define _device	 ((ID3D11Device*)GlobalDevice::get_device_ptr())
#define _context ((ID3D11DeviceContext*)GlobalDevice::get_context_ptr())

/*
--------------------------------------------------------------------------------------------
 Graphics class Internal Functions
--------------------------------------------------------------------------------------------
*/

// This structure contains all the data needed for the graphics
// obnect to render that is external for the user.
struct GraphicsInternals
{
	HWND HWnd = nullptr;

	ComPtr<IDXGISwapChain>			pSwap;
	ComPtr<ID3D11RenderTargetView>	pTarget;
	ComPtr<ID3D11DepthStencilView>	pDSV;
	ComPtr<ID3D11Buffer>			pPerspective;
};

// Destroys the class data and frees the pointers to the graphics instance.

Graphics::~Graphics()
{
	delete (GraphicsInternals*)GraphicsData;
}

// Initializes the class data and calls the creation of the graphics instance.
// Initializes all the necessary GPU data to be able to render the graphics objects.

Graphics::Graphics(void* hWnd)
{
	GlobalDevice::set_global_device();

	GraphicsData = new GraphicsInternals;
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	data.HWnd = (HWND)hWnd;
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0u;
	sd.BufferDesc.Height = 0u;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0u;
	sd.BufferDesc.RefreshRate.Denominator = 0u;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1u;
	sd.SampleDesc.Quality = 0u;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1u;
	sd.OutputWindow = (HWND)hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	ComPtr<IDXGIFactory> dxgiFactory;
	GFX_THROW_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), &dxgiFactory));

	//  Create swap chain

	GFX_THROW_INFO(dxgiFactory->CreateSwapChain(_device, &sd, data.pSwap.GetAddressOf()));

	//	Gain access to render target through shinnanigins

	ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(data.pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(_device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, data.pTarget.GetAddressOf()));

	//	Create and bind Perspective constant buffer

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(_float4matrix) + sizeof(_float4vector);
	cbd.StructureByteStride = 0u;
	GFX_THROW_INFO(_device->CreateBuffer(&cbd, NULL, data.pPerspective.GetAddressOf()));

	GFX_THROW_INFO_ONLY(_context->VSSetConstantBuffers(0u, 1u, data.pPerspective.GetAddressOf()));
}

/*
--------------------------------------------------------------------------------------------
 Graphics class Interface Functions
--------------------------------------------------------------------------------------------
*/

// Before issuing any draw calls to the window, for multiple window settings 
// this function has to be called to bind the window as the render target.

void Graphics::setRenderTarget()
{
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	// Bind the render target

	GFX_THROW_INFO_ONLY(_context->OMSetRenderTargets(1u, data.pTarget.GetAddressOf(), data.pDSV.Get()));

	// Bind the viewport

	CD3D11_VIEWPORT vp;
	vp.Width = (float)WindowDim.x;
	vp.Height = (float)WindowDim.y;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	GFX_THROW_INFO_ONLY(_context->RSSetViewports(1u, &vp));

	// Bind the perspective

	GFX_THROW_INFO_ONLY(_context->VSSetConstantBuffers(0u, 1u, data.pPerspective.GetAddressOf()));
}

// Swaps the current frame and shows the new frame to the window.

void Graphics::pushFrame()
{
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	HRESULT hr;

	if (FAILED(hr = data.pSwap->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(_device->GetDeviceRemovedReason());
		else
			throw GFX_EXCEPT(hr);
	}
}

// Clears the buffer with the specified color.

void Graphics::clearBuffer(Color color)
{
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	_float4color col = color.getColor4();
	GFX_THROW_INFO_ONLY(_context->ClearRenderTargetView(data.pTarget.Get(), &col.r));
	GFX_THROW_INFO_ONLY(_context->ClearDepthStencilView(data.pDSV.Get(),D3D11_CLEAR_DEPTH,1.f,0u));
}

// Clears the depth buffer so that all objects painted are moved to the back.

void Graphics::clearDepthBuffer()
{
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	GFX_THROW_INFO_ONLY(_context->ClearDepthStencilView(data.pDSV.Get(), D3D11_CLEAR_DEPTH, 1.f, 0u));
}

// Calls to draw the objects as indexed in the index count.

void Graphics::drawIndexed(unsigned IndexCount)
{
	GFX_THROW_INFO_ONLY(_context->DrawIndexed(IndexCount, 0u, 0u));
}

// Simple conversion from a pixel position on screen to a (-1.0,1.0)x(-1.0,1.0) c R^2 position.

Vector2f Graphics::PixeltoR2(const Vector2i MousePos)
{
	return Vector2f(2.f * MousePos.x / WindowDim.x - 1.f, -2.f * MousePos.y / WindowDim.y + 1.f);
}

/*
--------------------------------------------------------------------------------------------
 Getters & Setters
--------------------------------------------------------------------------------------------
*/

// Updates the perspective on the window, by changing the observer direction, 
// the center of the POV and the scale of the object looked at.

void Graphics::updatePerspective(Vector3f obs, Vector3f center, float scale)
{
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	if (!obs)
		throw INFO_EXCEPT("The observer must be a vector diferent than zero");

	Observer = obs.normalize();
	Center = center;
	Scale = scale;

	Matrix Projections;
	if (Observer == Vector3f(0.f, 0.f, -1.f))
		Projections = ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * scale;
	else
		Projections = ProjectionMatrix(obs) * ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * scale;

	cbuff.perspective = Projections.transpose().getMatrix4();
	cbuff.traslation = center.getVector4();

	GFX_THROW_INFO_ONLY(_context->UpdateSubresource(data.pPerspective.Get(), 0u, NULL, &cbuff, 0u, 0u));
}

// Sets the window dimensions to the ones specified by the vector.

void Graphics::setWindowDimensions(const Vector2i Dim)
{
	GraphicsInternals& data = *((GraphicsInternals*)GraphicsData);

	WindowDim = Dim;

	if (!WindowDim.x && !WindowDim.y)
		return;

	GFX_THROW_INFO_ONLY(_context->OMSetRenderTargets(0u, NULL, NULL));

	// Release references to old buffers
	data.pTarget.Reset();

	// Preserve the existing buffer count and format.
	// Automatically choose the width and height to match the client rect for HWNDs.

	GFX_THROW_INFO(data.pSwap->ResizeBuffers(0u, 0u, 0u, DXGI_FORMAT_UNKNOWN, 0u));

	// Get buffer and create a render-target-view.

	ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(data.pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
	GFX_THROW_INFO(_device->CreateRenderTargetView(pBackBuffer.Get(), NULL, &data.pTarget));

	//	Create and bind depth stencil state

	ComPtr<ID3D11DepthStencilState> pDSState;
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	GFX_THROW_INFO(_device->CreateDepthStencilState(&dsDesc, &pDSState));

	GFX_THROW_INFO_ONLY(_context->OMSetDepthStencilState(pDSState.Get(), 1u));

	//	Create and bind depth stencil texture

	ComPtr<ID3D11Texture2D> pDepthStencil;
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
	GFX_THROW_INFO(_device->CreateTexture2D(&descDepth, NULL, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(_device->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, data.pDSV.GetAddressOf()));

	//	Bind render target

	GFX_THROW_INFO_ONLY(_context->OMSetRenderTargets(1u, data.pTarget.GetAddressOf(), data.pDSV.Get()));

	CD3D11_VIEWPORT vp;
	vp.Width = (float)Dim.x;
	vp.Height = (float)Dim.y;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	GFX_THROW_INFO_ONLY(_context->RSSetViewports(1u, &vp));

	//	Update perspective to match scaling

	Matrix Projections;
	if (Observer == Vector3f(0.f, 0.f, -1.f))
		Projections = ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * Scale;
	else
		Projections = ProjectionMatrix(Observer) * ScalingMatrix(1.f / WindowDim.x, 1.f / WindowDim.y, 1.f) * Scale;

	cbuff.perspective = Projections.transpose().getMatrix4();

	GFX_THROW_INFO_ONLY(_context->UpdateSubresource(data.pPerspective.Get(), 0u, NULL, &cbuff, 0u, 0u));

	clearDepthBuffer();
}

// Returns the current window dimensions as an interger vector.

Vector2i Graphics::getWindowDimensions() const
{
	return WindowDim;
}

// Returns the current observer direction vector.

Vector3f Graphics::getObserver() const
{
	return Observer;
}

// Returns the current Center POV.

Vector3f Graphics::getCenter() const
{
	return Center;
}

// Returns the current scals.

float Graphics::getScale() const
{
	return Scale;
}
