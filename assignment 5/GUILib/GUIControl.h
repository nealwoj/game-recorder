#pragma once

#include <Trackable.h>
#include <Defines.h>
#include <Color.h>
#include "Vector2D.h"
#include "GraphicsBuffer.h"
#include <functional>

class GraphicsBuffer;

using namespace std;

typedef IDType ControlID;
typedef function<void(const ControlID&)> ControlCallback;

class GUIControl :public Trackable
{
public:
	GUIControl();
	GUIControl(ControlID theID, Vector2D loc, Vector2D size, ControlCallback& pressedCB, ControlCallback& releasedCB, ControlCallback& hoverCB);
	~GUIControl() {};

	void draw(GraphicsBuffer* dest);//implement me!
	//void setCurrentColor(Color& color) { mCurrentColor = color; };
	void setHover() { mCurrentColor = mHoverColor; };
	void setPressed() { mCurrentColor = mPressedColor; };
	void setReleased() { mCurrentColor = mBackgroundColor; };

	void onClick(string type);

	Vector2D getLoc() { return mLocation; };
	Vector2D getSize() { return mSize; };
	ControlID getID() { return mID; };
	Color getCurrentColor() { return mCurrentColor; };

private:
	ControlID mID;
	//xy location
	Vector2D mLocation;
	//size
	Vector2D mSize;
	//text
	string mText;
	//functions to call (onPress, onRelease)
	ControlCallback mPressedCallback;
	ControlCallback mReleasedCallback;
	ControlCallback mHoverCallback;
	//background color
	Color mBackgroundColor;
	//text color
	Color mTextColor;
	//hover color
	Color mHoverColor; 
	//pressed color
	Color mPressedColor;
	Color mCurrentColor;
};
