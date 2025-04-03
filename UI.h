#ifndef _UI_
#define _UI_

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>
#include "Vertex.h"
#include <unordered_map>

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

#endif // !_UI_

