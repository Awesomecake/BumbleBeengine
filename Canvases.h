#ifndef _Canvases_
#define _Canvases_

#include "UICanvas.h"

class TestCanvas : public UICanvas
{
public:
	TestCanvas();
	~TestCanvas();
	void Update() override;
	void Render() override;
	void OnOpen() override;
	void OnClose() override;
};

#endif // _Canvases_
