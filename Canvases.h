#ifndef _Canvases_
#define _Canvases_

#include "UICanvas.h"

#include <string>

using namespace ImGui;

#define CANVAS_FIELDS(CLASS_NAME)	\
public:								\
	CLASS_NAME(std::string name);	\
	~CLASS_NAME();					\
	void Update() override;			\
	void Render() override;			\
	void OnOpen() override;			\
	void OnClose() override;		\

class TestCanvas : public UICanvas 
{ 
	CANVAS_FIELDS(TestCanvas) 
public:
	int timesClosed = 0;
};

class Canvas_TopBar : public UICanvas 
{
	CANVAS_FIELDS(Canvas_TopBar) 
public:
	int timesOpened = 0;
};

class Canvas_Clock : public UICanvas
{
	CANVAS_FIELDS(Canvas_Clock)
public:
	std::string clockTime;
};

#endif // _Canvases_
