#include "GameRecorder.h"
#include "Game.h"
#include "UnitManager.h"
#include "SpriteManager.h"

GameRecorder::GameRecorder()
{
	mRecording = false;
	mCount = 0;
}

void GameRecorder::cleanup()
{
	mCount = 0;
	mRecording = false;
	mDeltaTime = 0.0;
}


void GameRecorder::startRecording()
{
	//current time?
	mDeltaTime = gpGame->getCurrentTime();

	mBinaryInput.open(BINARY_FILENAME, ios::binary);
	mTextOutput.open(TEXT_FILENAME);

	mCount = 0;
	mRecording = true;
}

void GameRecorder::stopRecording()
{
	mBinaryInput.close();
	mTextOutput.close();

	mDeltaTime = 0.0f;
	mRecording = false;
}

void GameRecorder::handleEvent(const Event& theEvent)
{
	if (mRecording)
	{
		write(theEvent);
	}
	else
	{
		//fire events that come from playback
		if (theEvent.getType() == Event::MOUSE_BUTTON_EVENT)
		{
			Unit* pUnit = gpGame->getUnitManager()->createRandomUnit(*gpGame->getSpriteManager()->getSprite(AI_ICON_SPRITE_ID));
		}
		else if (theEvent.getType() == Event::KEYBOARD_EVENT)
		{
			Unit* pUnit = gpGame->getUnitManager()->createRandomUnit(*gpGame->getSpriteManager()->getSprite(AI_ICON_SPRITE_ID));
			//Unit* pUnit = gpGame->getUnitManager()->createRandomUnit(*gpGame->getSpriteManager()->getSprite(AI_ICON_SPRITE_ID));
		}
	}
}

void GameRecorder::write(const Event& theEvent)
{
	//write event to output file
	mDeltaTime = gpGame->getCurrentTime() - mDeltaTime;

	if (theEvent.getType() == Event::MOUSE_BUTTON_EVENT)
	{
		const MouseButtonEvent& iEvent = static_cast<const MouseButtonEvent&>(theEvent);
		mTextOutput << "MouseButton" << endl;
		mBinaryInput.write((const char*)(&iEvent), sizeof(iEvent));
	}
	else if (theEvent.getType() == Event::MOUSE_MOVE_EVENT)
	{
		const MouseMoveEvent& iEvent = static_cast<const MouseMoveEvent&>(theEvent);
		mTextOutput << "MouseMove" << endl;
		mBinaryInput.write((const char*)(&iEvent), sizeof(iEvent));
	}
	else if (theEvent.getType() == Event::KEYBOARD_EVENT)
	{
		const KeyEvent& iEvent = static_cast<const KeyEvent&>(theEvent);
		if (iEvent.getKey() != InputSystem::R_KEY && iEvent.getKey() != InputSystem::S_KEY && iEvent.getKey() != InputSystem::P_KEY && iEvent.getKey() != InputSystem::E_KEY)
		{
			mTextOutput << "Keyboard" << endl;
			mBinaryInput.write((const char*)(&iEvent), sizeof(iEvent));
		}
	}

	mCount++;
}