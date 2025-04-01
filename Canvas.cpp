#include "Canvas.h"

namespace UI
{
	Canvas::Canvas()
	{
	}

	Canvas::~Canvas()
	{
		for (auto& object : objects)
		{
			delete object.second;
		}
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

	float Canvas::GetPosition(AnchorPoint anchor, XMFLOAT2 position)
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
		return 0.0f;
	}
}