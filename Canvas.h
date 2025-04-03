#ifndef _Canvas_
#define _Canvas_

#include "UI.h"
#include "CanvasObject.h"

class Canvas
{
public:
	// Constructors
	Canvas();
	~Canvas();

	// Public Variables
	XMFLOAT2 size;   // The size of the canvas in 2D space
	XMFLOAT2 origin; // The origin point of the canvas

	// Public Methods
	virtual void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
	virtual void OnResize(float width, float height);
	inline Canvas* GetObject(wchar_t* name) { return objects[name]; }
	virtual inline void AddObject(wchar_t* name, Canvas* object)
	{
		object->parent = this; // Set the parent of the object to this canvas

		// Do not add if the object already exists
		if (objects.find(name) != objects.end())
		{
			// Optionally handle the case where the object already exists
			// e.g., log a message or throw an exception
			return;
		}
		objects[name] = object; // Add the new object
	}

protected:
	// Protected Variables
	Canvas* parent; // Pointer to the parent CanvasObject, if any

	// Protected Methods
	float GetScreenWidth();
	float GetScreenHeight();
	XMFLOAT2 GetPosition(AnchorPoint anchor, XMFLOAT2 position);
private:
	// Private Variables
	std::unordered_map<wchar_t*, Canvas*> objects;
};

#endif // !_Canvas_