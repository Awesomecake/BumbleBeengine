#ifndef _UI_
#define _UI_

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>
#include "Vertex.h"
#include <unordered_map>

using namespace DirectX;

namespace UI
{
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
		inline CanvasObject()
		{
			anchorPoint = AnchorPoint::Center;
			position = XMFLOAT2(0, 0);
			indexCount = 0;
			RefreshObject();
		}
		~CanvasObject();

		// Public Variables
		AnchorPoint anchorPoint; // The anchor point of the object
		XMFLOAT2 position; // The position of the object in 2D space

		// Public Methods
		void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
		inline void RefreshObject() { Draw(); } // Refresh the object's buffers
		inline void AppendChild(CanvasObject* child) { children.push_back(child); } // Add a child object to the list
	protected:
		// Protected Variables
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

	class Canvas
	{
	public:
		// Constructors
		Canvas();
		~Canvas();

		// Public Methods
		virtual void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
		inline CanvasObject* GetObject(wchar_t* name) { return objects[name]; }

	protected:
		// Protected Methods
		float GetScreenWidth();
		float GetScreenHeight();
		float GetPosition(AnchorPoint, XMFLOAT2);
	private:
		// Private Variables
		std::unordered_map<wchar_t*, CanvasObject*> objects;
	};
}


#endif // !_UI_

