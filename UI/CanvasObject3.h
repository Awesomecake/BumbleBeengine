#pragma once

#ifndef _CanvasObject_
#define _CanvasObject_

#include "UI.h"
#include "Canvas.h"

//class CanvasObject /*: public Canvas*/  // Inherit from Canvas to allow for rendering in the canvas
//{
//public:
//	// Constructor and Destructor
//	CanvasObject();
//	~CanvasObject();
//
//	// Public Variables
//	AnchorPoint anchorPoint; // The anchor point of the object
//	XMFLOAT2 position; // The position of the object in 2D space
//
//	// Public Methods
//	/*void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
//	virtual void OnResize(float width, float height) override;*/
//protected:
//	// Protected Variables
//	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
//	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
//	unsigned int indexCount;
//
//	// protected Methods
//	virtual void Draw() = 0;
//private:
//	// Private Variables
//
//	// Private Methods
//	void SendToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
//};


#endif // !_CanvasObject_
