#include "CanvasObject.h"

CanvasObject::CanvasObject()
{
	anchorPoint = AnchorPoint::Center;
	position = XMFLOAT2(0, 0);
	indexCount = 0;
}

CanvasObject::~CanvasObject()
{
	
}

//void CanvasObject::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
//{
//	SendToGPU(context); // Send the object to the GPU for rendering
//	__super::Render(context); // Call the base class Render to render child objects
//}
//
//void CanvasObject::OnResize(float width, float height)
//{
//	origin.x = parent->origin.x / 2.0f; // Set the origin to the center of the parent canvas
//	origin.y = parent->origin.y / 2.0f; // Set the origin to the center of the parent canvas
//
//	// Update the position based on the anchor point
//	position = parent->GetPosition(anchorPoint, position);
//}

void CanvasObject::SendToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	//Load Buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Tell Direct3D to draw
	//  - Begins the rendering pipeline on the GPU
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		indexCount,     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}