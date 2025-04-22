#include "Canvases.h"
#include <iostream>

void TestCanvas::Render()
{
	ImGui::Begin("Test");
	ImGui::Text("This is a test canvas.");
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

}

TestCanvas::TestCanvas()
{
}

TestCanvas::~TestCanvas()
{
}