#ifndef _UICanvas_
#define _UICanvas_

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

using namespace std;

class UICanvas
{
public:
	UICanvas();
	~UICanvas();
	bool isRendering = false;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void OnOpen() = 0;
	virtual void OnClose() = 0;
};

#endif // _UICanvas_
