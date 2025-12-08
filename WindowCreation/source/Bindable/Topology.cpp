#include "Bindable/Topology.h"
#include "WinHeader.h"
#include "Graphics.h"
#include "Exception/_exGraphics.h"

#define _device ((ID3D11Device*)device())
#define _context ((ID3D11DeviceContext*)context())

Topology::Topology(unsigned type)
	: type(type)
{
}

void Topology::Bind()
{
	GFX_THROW_INFO_ONLY(_context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)type));
}
