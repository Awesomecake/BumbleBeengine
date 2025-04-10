#include "Canvas.h"

Canvas::Canvas()
{
}

Canvas::~Canvas()
{
	for (auto& object : objects)
	{
		delete object.first;
		delete object.second;
	}
	objects.clear(); // Clear the unordered_map of objects
}

float Canvas::GetScreenWidth()
{
	return 0.0f;
}

float Canvas::GetScreenHeight()
{
	return 0.0f;
}

void Canvas::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	for (auto& object : objects)
	{
		object.second->Render(context);
	}
}

void Canvas::OnResize(float width, float height)
{
	origin.x = width / 2.0f; // Set the origin to the center of the canvas
	origin.y = height / 2.0f; // Set the origin to the center of the canvas
}

XMFLOAT2 Canvas::GetPosition(AnchorPoint anchor, XMFLOAT2 position)
{
	switch (anchor)
	{
	case Top:
		break;
	case Bottom:
		break;
	case Left:
		break;
	case Right:
		break;
	case Center:
		break;
	case TopLeft:
		break;
	case TopRight:
		break;
	case BottomLeft:
		break;
	case BottomRight:
		break;
	default:
		break;
	}
	return XMFLOAT2();
}