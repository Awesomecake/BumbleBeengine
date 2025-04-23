#include "Canvases.h"
#include <format>

void TestCanvas::Render()
{
	ImGui::Begin(Name.c_str());
	Text(std::format("This window has been closed {} times", timesClosed).c_str());
	ImGui::End();
}

void TestCanvas::Update()
{
}

void TestCanvas::OnOpen()
{
	
}

void TestCanvas::OnClose()
{
	timesClosed += 1;
}

TestCanvas::TestCanvas(std::string name)
	: UICanvas(name)
{

}

TestCanvas::~TestCanvas()
{
}