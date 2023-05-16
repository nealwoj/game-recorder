#include "GamePlayer.h"
#include "Game.h"
#include <string>
#include <sstream>

GamePlayer::GamePlayer()
{
	mpEventPlayback = new CircularQueue<Event*>(0);
	mPlaying = false;
}

void GamePlayer::cleanup()
{
	delete mpEventPlayback;
	mpEventPlayback = NULL;
	mPlaying = false;
}

void GamePlayer::startPlayback()
{
	mPlaying = true;
	Event* pEvent;

	while (mpEventPlayback->popFront(pEvent))
	{
		if (pEvent->getType() != Event::INVALID_EVENT_TYPE)
		{
			EventSystem::getInstance()->fireEvent(*pEvent);
		}
	}
}

void GamePlayer::stopPlayback()
{
	mpEventPlayback->reset();
	mPlaying = false;
}

void GamePlayer::load()
{
	ifstream textInput, binInput;

	binInput.open(BINARY_FILENAME, ios::binary);
	textInput.open(TEXT_FILENAME);
	if (textInput && binInput)
	{
		int numOfEvents = gpGame->getRecorderCount();

		delete mpEventPlayback;
		mpEventPlayback = new CircularQueue<Event*>(numOfEvents);

		for (int i = 0; i < numOfEvents; i++)
		{
			Event* pEvent = new Event(Event::INVALID_EVENT_TYPE);
			string type;

			getline(textInput, type);

			if (type == "MouseButton")
				binInput.read((char*)(pEvent), BUTTON_BYTE_SIZE);
			else if (type == "MouseMove")
				binInput.read((char*)(pEvent), MOVE_BYTE_SIZE);
			else if (type == "Keyboard")
				binInput.read((char*)(pEvent), KEYBOARD_BYTE_SIZE);

			mpEventPlayback->pushBack(pEvent);
		}
	}

	textInput.close();
	binInput.close();
}