#include "Set.h"

Set::Set(Graphics& gfx)
{
	isInit = true;

	_float4vector vertexs[4] = {
		{ -1.f,-1.f,1.f,1.f },
		{  1.f,-1.f,1.f,1.f },
		{ -1.f, 1.f,1.f,1.f },
		{  1.f, 1.f,1.f,1.f },
	};

	AddBind(std::make_unique<VertexBuffer>(gfx, vertexs, 4u));

	unsigned int indexs[6] = { 0u,2u,1u,1u,2u,3u };

	AddBind(std::make_unique<IndexBuffer>(gfx, indexs, 6u));

	VertexShader* pvs;
	pvs = (VertexShader*)AddBind(std::move(std::make_unique<VertexShader>(gfx, SHADERS_DIR L"SetVS.cso")));
	AddBind(std::make_unique<PixelShader>(gfx, SHADERS_DIR L"SetPS.cso"));

	PcBuff.color[0] = { 0.f,0.f,0.f,0.f };
	PcBuff.color[1] = { 1.f,1.f,1.f,160.f };
	PcBuff.color[2] = { 1.f,0.f,1.f,140.f };
	PcBuff.color[3] = { 0.f,0.5f,1.f,120.f };
	PcBuff.color[4] = { 0.5f,0.f,1.f,100.f };
	PcBuff.color[5] = { 1.f,0.f,0.f,80.f };
	PcBuff.color[6] = { 1.f,0.5f,0.f,70.f };
	PcBuff.color[7] = { 1.f,1.f,0.f,60.f };
	PcBuff.color[8] = { 0.5f,0.5f,0.5f,50.f };
	PcBuff.color[9] = { 0.8f,0.2f,0.f,40.f };
	PcBuff.color[10] = { 0.5f,0.1f,0.7f,30.f };
	PcBuff.color[11] = { 0.f,0.6f,0.9f,20.f };
	PcBuff.color[12] = { 0.7f,1.f,0.f,10.f };
	PcBuff.color[13] = { 0.8f,0.6f,0.f,5.f };
	PcBuff.color[14] = { 0.4f,0.7f,0.f,2.f };
	PcBuff.color[15] = { 1.f,1.f,0.f,0.f };

	pscBuff = (ConstantBuffer<PSconstBuffer>*)AddBind(std::make_unique<ConstantBuffer<PSconstBuffer>>(gfx, PcBuff, PIXEL_CONSTANT_BUFFER_TYPE));

	D3D11_INPUT_ELEMENT_DESC ied[1] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	AddBind(std::make_unique<InputLayout>(gfx, ied, 1u, pvs->GetBytecode()));
	AddBind(std::make_unique<Topology>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_unique<Rasterizer>(gfx, false));
	AddBind(std::make_unique<Blender>(gfx, false));
	AddBind(std::make_unique<Sampler>(gfx, D3D11_FILTER_MIN_MAG_MIP_LINEAR));
}

void Set::updateGradient(Graphics& gfx, Color color, float tolerance, unsigned char ID)
{
	PcBuff.color[ID] = color.getColor4();
	PcBuff.color[ID].a = tolerance;
	pscBuff->Update(gfx, PcBuff);
}
