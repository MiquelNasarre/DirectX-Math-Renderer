#include "Set.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

void Set::saveFrameAsync(PSconstBuffer cb, const char* filename, Complex start, float dx, float dy, Vector2i Dimensions)
{
    return;

	//  Create Factory

	pCom<IDXGIFactory> dxgiFactory;

	CreateDXGIFactory(__uuidof(IDXGIFactory), &dxgiFactory);
	pCom<IDXGIFactory6> Factory6;
	dxgiFactory.As(&Factory6);

	//  Find adapter by GPU preference

	pCom<IDXGIAdapter> bestAdapter = nullptr;

    HRESULT hr = Factory6->EnumAdapterByGpuPreference(
        0, 
        DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, 
        IID_PPV_ARGS(&bestAdapter)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to find a D3D12 adapter.");

	//  Create D3D12 device with the chosen adapter

    ComPtr<ID3D12Device> pDevice;

    hr = D3D12CreateDevice(
        bestAdapter.Get(),          // Chosen adapter
        D3D_FEATURE_LEVEL_12_0,     // Minimum feature level
        IID_PPV_ARGS(&pDevice)       // Output device
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create D3D12 device.");

    // Create Command Queue

    ComPtr<ID3D12CommandQueue> commandQueue;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

    hr = pDevice->CreateCommandQueue(
        &queueDesc, 
        IID_PPV_ARGS(&commandQueue)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create command queue.");

    // Create Command Allocator

    ComPtr<ID3D12CommandAllocator> commandAllocator;

    hr = pDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_COMPUTE, 
        IID_PPV_ARGS(&commandAllocator)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create command allocator.");

    // Create Command List

    ComPtr<ID3D12GraphicsCommandList> commandList;

    hr = pDevice->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        commandAllocator.Get(),
        nullptr, // Initial pipeline state
        IID_PPV_ARGS(&commandList)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create command list.");

    // 2. Create Texture Resource (UAV)

    ComPtr<ID3D12Resource> mandelbrotTexture;

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Width = Dimensions.x;
    textureDesc.Height = Dimensions.y;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_HEAP_PROPERTIES prop0(D3D12_HEAP_TYPE_DEFAULT);
    hr = pDevice->CreateCommittedResource(
        &prop0, // GPU memory
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS, // Initial state
        nullptr,
        IID_PPV_ARGS(&mandelbrotTexture)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create texture resource.");

    // 3. Create Readback Buffer

    ComPtr<ID3D12Resource> readbackBuffer;

    UINT64 imageSize = Dimensions.x * Dimensions.y * 4; // 4 bytes per pixel (R8G8B8A8)
    D3D12_RESOURCE_DESC readbackDesc = CD3DX12_RESOURCE_DESC::Buffer(imageSize);

    CD3DX12_HEAP_PROPERTIES prop1(D3D12_HEAP_TYPE_READBACK);
    hr = pDevice->CreateCommittedResource(
        &prop1, // CPU accessible
        D3D12_HEAP_FLAG_NONE,
        &readbackDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&readbackBuffer)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create readback buffer.");

    // 4. Create Descriptor Heap for UAV

    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = pDevice->CreateDescriptorHeap(
        &heapDesc, 
        IID_PPV_ARGS(&descriptorHeap)
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to create descriptor heap.");

    // Create UAV for the texture

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = textureDesc.Format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    pDevice->CreateUnorderedAccessView(
        mandelbrotTexture.Get(), 
        nullptr, 
        &uavDesc, 
        descriptorHeap->GetCPUDescriptorHandleForHeapStart()
    );

    // 5. Compile Compute Shader

    ComPtr<ID3DBlob> pBytecodeBlob;
    
    hr = D3DReadFileToBlob(
        SHADERS_DIR L"SetCS.cso", 
        &pBytecodeBlob
    );
    if (FAILED(hr)) throw std::runtime_error("Failed to read shader file.");

    // 6. Create Root Signature

    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;
    D3D12_ROOT_PARAMETER rootParameters[1] = {};

    CD3DX12_DESCRIPTOR_RANGE dr(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
    // Parameter 0: UAV
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &dr;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = _countof(rootParameters);
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED; //D3D12_ROOT_SIGNATURE_FLAG_ALLOW_COMPUTE_ROOT_DESCRIPTOR_TABLE;

    hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSignatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        throw std::runtime_error("Failed to serialize root signature.");
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    hr = pDevice->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) throw std::runtime_error("Failed to create root signature.");

    // 7. Create Compute Pipeline State
    D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc = {};
    pipelineStateDesc.pRootSignature = rootSignature.Get();
    pipelineStateDesc.CS = CD3DX12_SHADER_BYTECODE(pBytecodeBlob.Get());
    pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

    ComPtr<ID3D12PipelineState> pipelineState;
    hr = pDevice->CreateComputePipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) throw std::runtime_error("Failed to create pipeline state.");

    // 8. Set Up Command List
    commandList->SetComputeRootSignature(rootSignature.Get());
    commandList->SetPipelineState(pipelineState.Get());

    // Set Descriptor Heaps
    ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    // Set Compute Root Descriptor Table
    D3D12_GPU_DESCRIPTOR_HANDLE uavHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    commandList->SetComputeRootDescriptorTable(0, uavHandle);

    // 9. Transition Texture to UAV
    D3D12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        mandelbrotTexture.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );
    commandList->ResourceBarrier(1, &uavBarrier);

    // 10. Dispatch Compute Shader
    UINT groupSizeX = (Dimensions.x + 15) / 16; // Assuming [numthreads(16, 16, 1)]
    UINT groupSizeY = (Dimensions.y + 15) / 16;
    commandList->Dispatch(groupSizeX, groupSizeY, 1);

    // 11. Transition Texture to Copy Source
    D3D12_RESOURCE_BARRIER copyBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        mandelbrotTexture.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_COPY_SOURCE
    );
    commandList->ResourceBarrier(1, &copyBarrier);

    // 12. Copy Texture to Readback Buffer
    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = mandelbrotTexture.Get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    srcLocation.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = readbackBuffer.Get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dstLocation.PlacedFootprint.Footprint.Format = textureDesc.Format;
    dstLocation.PlacedFootprint.Footprint.Width = Dimensions.x;
    dstLocation.PlacedFootprint.Footprint.Height = Dimensions.y;
    dstLocation.PlacedFootprint.Footprint.Depth = 1;
    dstLocation.PlacedFootprint.Footprint.RowPitch = Dimensions.x * 4; // 4 bytes per pixel

    commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

    // 13. Close Command List and Execute
    hr = commandList->Close();
    if (FAILED(hr)) throw std::runtime_error("Failed to close command list.");

    ID3D12CommandList* listsToExecute[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(listsToExecute), listsToExecute);

    // 14. Create Fence and Event for Synchronization
    ComPtr<ID3D12Fence> fence;
    hr = pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (FAILED(hr)) throw std::runtime_error("Failed to create fence.");

    UINT64 fenceValue = 1;
    hr = commandQueue->Signal(fence.Get(), fenceValue);
    if (FAILED(hr)) throw std::runtime_error("Failed to signal fence.");

    // Create an event handle for fence completion
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) throw std::runtime_error("Failed to create fence event.");

    // Wait for the fence to be completed
    if (fence->GetCompletedValue() < fenceValue)
    {
        hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
        if (FAILED(hr)) throw std::runtime_error("Failed to set fence event.");
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    CloseHandle(fenceEvent);

    // 15. Map Readback Buffer and Save Image
    void* pData = nullptr;
    D3D12_RANGE readRange = { 0, imageSize };
    hr = readbackBuffer->Map(0, &readRange, &pData);
    if (FAILED(hr)) throw std::runtime_error("Failed to map readback buffer.");

    // Copy data to a vector
    std::vector<BYTE> imageData(imageSize);
    memcpy(imageData.data(), pData, imageSize);

    readbackBuffer->Unmap(0, nullptr);
    
}
