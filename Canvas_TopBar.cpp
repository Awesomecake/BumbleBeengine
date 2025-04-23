#include "Canvases.h"
#include <format>

void Canvas_TopBar::Render()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize 
		| ImGuiWindowFlags_NoCollapse 
		| ImGuiWindowFlags_AlwaysAutoResize 
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar;

	SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 0), ImGuiCond_Always);

	Begin(Name.c_str(), nullptr, flags);
	
	Text(std::format("Topbar has been opened {} times", timesOpened).c_str());
	SameLine();
	Button("Button 1");
	SameLine();
	Button("Button 2");
	SameLine();
	Button("Button 3");
	SameLine();
	Button("Button 3");

	End();
}

void Canvas_TopBar::Update()
{
}

void Canvas_TopBar::OnOpen()
{
	timesOpened += 1;
}

void Canvas_TopBar::OnClose()
{

}

Canvas_TopBar::Canvas_TopBar(std::string name)
	: UICanvas(name)
{

}

Canvas_TopBar::~Canvas_TopBar()
{
}