#include "UIManager.h"

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	for (auto& canvas : canvasStack)
	{
		delete canvas;
	}
	canvasStack.clear();

	for (auto& pair : uiElements)
	{
		delete pair.second;
	}
	uiElements.clear();
}

void UIManager::Render()
{
	for (auto& canvas : canvasStack)
	{
		canvas->Update();
		canvas->Render();
	}
}

void UIManager::Update()
{	for (auto& canvas : canvasStack)
	{
		canvas->Update();
	}
}

void UIManager::OpenCanvas(const string& canvasName)
{
	auto keyValue = uiElements.find(canvasName);
	if (keyValue != uiElements.end())
	{
		UICanvas* canvas = keyValue->second;

		if (canvas->isRendering)
		{
			// Handle the case where the canvas is already open
			return;
		}

		canvas->OnOpen();
		canvas->isRendering = true;
		canvasStack.push_back(canvas);
	}
}

void UIManager::CloseCanvas(const string& canvasName)
{
	auto keyValue = uiElements.find(canvasName);
	if (keyValue != uiElements.end())
	{
		UICanvas* canvas = keyValue->second;

		if (!canvas->isRendering)
		{
			// Handle the case where the canvas is not open
			return;
		}

		canvas->OnClose();
		canvas->isRendering = false;
		canvasStack.erase(remove(canvasStack.begin(), canvasStack.end(), canvas), canvasStack.end());
	}
}
