#ifndef _CanvasObject_
#define _CanvasObject_

#include <DirectXMath.h>
#include <vector>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <d3d11.h>
#include "Vertex.h"

using namespace DirectX;

enum AnchorPoint
{
	Top,
	Bottom,
	Left,
	Right,
	Center,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
};

class CanvasObject
{
public:
	// Constructor and Destructor
	inline CanvasObject() { anchorPoint = AnchorPoint::Center; position = XMFLOAT2(0, 0);  indexCount = 0; }
	~CanvasObject();

	// Public Methods
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	inline void AppendChild(CanvasObject* child) { children.push_back(child); } // Add a child object to the list
	
protected:
	// Protected Variables
	AnchorPoint anchorPoint; // The anchor point of the object
	XMFLOAT2 position; // The position of the object in 2D space
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	unsigned int indexCount;

	// protected Methods
	virtual void Draw() = 0;
private:
	// Private Variables
	std::vector<CanvasObject*> children; // List of child objects

	// Private Methods
	void SendToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

// Implementation
inline CanvasObject::~CanvasObject()
{
	for (CanvasObject* child : children) // Clean up child objects
	{
		delete child;
	}
	children.clear(); // Clear the vector of child objects
}

inline void CanvasObject::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	Draw();
	SendToGPU(context); // Send the object to the GPU for rendering
	for (CanvasObject* child : children) // Iterate through children and render them
	{
		child->Render(context);
	}
}

inline void CanvasObject::SendToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
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

#endif // !_CanvasObject_
