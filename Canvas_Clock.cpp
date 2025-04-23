#include "Canvases.h"

#include <format>

#include <ctime>
#include <iomanip>
#include <chrono>

void Canvas_Clock::Render()
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_AlwaysAutoResize;

	SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);

	ImGui::Begin(Name.c_str(), nullptr, flags);
	Text(clockTime.c_str());
	ImGui::End();
}

void Canvas_Clock::Update()
{
	// Get the current system time
	auto now = std::chrono::system_clock::now();

	// Convert it to a time_t object
	time_t convertedTime = std::chrono::system_clock::to_time_t(now);

	char timeBuffer[30];

	ctime_s(timeBuffer, sizeof(timeBuffer), &convertedTime);

	char* time = timeBuffer;

	clockTime = std::string(time);
}

void Canvas_Clock::OnOpen()
{
}

void Canvas_Clock::OnClose()
{
}

Canvas_Clock::Canvas_Clock(std::string name)
	: UICanvas(name)
{
}

Canvas_Clock::~Canvas_Clock()
{
}