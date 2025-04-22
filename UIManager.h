#ifndef _UIManager_
#define _UIManager_

#include<unordered_map>
#include<string>
#include<vector>

#include "UICanvas.h"

using namespace std;

class UIManager
{
public:
	UIManager();
	~UIManager();
	void Render();
	void Update();
	void OpenCanvas(const string& canvasName);
	void CloseCanvas(const string& canvasName);
	inline void addCanvas(const string& canvasName, UICanvas* canvas)
	{
		uiElements[canvasName] = canvas;
	}
	inline void removeCanvas(const string& canvasName)
	{
		auto keyValue = uiElements.find(canvasName);
		if (keyValue != uiElements.end())
		{
			delete keyValue->second;
			uiElements.erase(keyValue);
		}
	}
private:
	vector<UICanvas*> canvasStack;
	unordered_map<string, UICanvas*> uiElements;
};

#endif // !_UIManager_
