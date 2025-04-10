#ifndef _CanvasObject_
#define _CanvasObject_

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include <unordered_map>

using namespace DirectX;

// Base class for all objects that are rendered as UI in the program
class CanvasObject
{
public:
	// Constructor and Destructor
	CanvasObject();
	~CanvasObject();

	// Public Variables

	// Public Methods
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	inline void RefreshObject() { Draw(); } // Refresh the object's buffers
	inline void AppendChild(const wchar_t* name, CanvasObject* child);
	inline CanvasObject* GetChild(const wchar_t* name) { return objects[name]; }
protected:
	// Protected Variables
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	unsigned int indexCount;

	// Protected Methods
	virtual void Draw() = 0;
private:
	// Private Variables
	std::unordered_map<wchar_t*, CanvasObject*> objects;

	// Private Methods
	void SendToGPU(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

inline void CanvasObject::AppendChild(const wchar_t* name, CanvasObject* child)
{
	objects.emplace(name, child);
}


#endif // !_CanvasObject_
