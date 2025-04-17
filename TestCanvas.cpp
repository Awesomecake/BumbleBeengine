#include "UICanvas.h"

class TestCanvas : public UICanvas
{
public:
	TestCanvas();
	~TestCanvas();
};

void UICanvas::Render()
{
	ImGui::Begin(name.c_str(), &isRendering);
	ImGui::Text("This is a test canvas.");
	ImGui::End();
}

void UICanvas::Update()
{
	
}

void UICanvas::OnOpen()
{
	
}

void UICanvas::OnClose()
{

}

TestCanvas::TestCanvas()
{
}

TestCanvas::~TestCanvas()
{
}