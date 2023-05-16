#pragma once

#include <Trackable.h>
#include <EventListener.h>
#include <vector>
#include <Event.h>

class GUIControl;
class GraphicsBuffer;

typedef int ControlID;

class GUIPanel :public EventListener
{
public:
	GUIPanel();
	~GUIPanel() { clear(); };
	void clear();

	void addControl(GUIControl* pControl);
	void draw(GraphicsBuffer* dest);

	void press(const ControlID& id);
	void release(const ControlID& id);
	void hover(const ControlID& id);

private:
	std::vector<GUIControl*> mpControls;

	void handleEvent(const Event& theEvent);
};
