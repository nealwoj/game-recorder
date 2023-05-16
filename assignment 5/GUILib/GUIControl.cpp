#include "GUIControl.h"
#include <GraphicsSystem.h>

//GUIControl::GUIControl(ControlID theID)
//	:mID(theID)
//{
//	//just a test
//	GraphicsSystem::getBackBuffer();
//}

GUIControl::GUIControl()
{
}

GUIControl::GUIControl(ControlID theID, Vector2D loc, Vector2D size, ControlCallback& pressedCB, ControlCallback& releasedCB, ControlCallback& hoverCB)
{
	mID = theID;
	mLocation = loc;
	mSize = size;

	mBackgroundColor = Color(255.0f, 255.0f, 255.0f);
	mTextColor = Color(255.0f, 0.0f, 0.0f);
	mHoverColor = Color(0.0f, 255.0f, 0.0f);
	mPressedColor = Color(0.0f, 0.0f, 255.0f);

	mPressedCallback = pressedCB;
	mReleasedCallback = releasedCB;
	mHoverCallback = hoverCB;

	mCurrentColor = Color(255.0f, 255.0f, 255.0f);
}

void GUIControl::draw(GraphicsBuffer* dest)
{
	GraphicsSystem::renderRect(*dest, mLocation, (int)mSize.getX(), (int)mSize.getY(), Color(255.0f, 0.0f, 0.0f));
}

void GUIControl::onClick(string type)
{
	//calls the callbacks here
	if (type == "pressed")
	{
		mPressedCallback(mID);
	}
	else if (type == "released")
	{
		mReleasedCallback(mID);
	}
	else if (type == "hover")
	{
		mHoverCallback(mID);
	}
}