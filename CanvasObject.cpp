#include "CanvasObject.h"

namespace UI
{
	CanvasObject::~CanvasObject()
	{
		for (CanvasObject* child : children) // Clean up child objects
		{
			delete child;
		}
		children.clear(); // Clear the vector of child objects
	}

	void CanvasObject::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
	{
		SendToGPU(context); // Send the object to the GPU for rendering
		for (CanvasObject* child : children) // Iterate through children and render them
		{
			child->Render(context);
		}
	}

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
}