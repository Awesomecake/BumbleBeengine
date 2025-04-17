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
private:
	vector<UICanvas*> canvasStack;
	unordered_map<string, UICanvas*> uiElements;
};

#endif // !_UIManager_
