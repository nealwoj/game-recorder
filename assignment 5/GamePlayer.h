#pragma once

#include <Trackable.h>
#include <CircularQueue.h>
#include <EventSystem.h>
#include <fstream>
#include "Event.h"
#include <InterfaceEvents.h>

using namespace std;

class GamePlayer : public Trackable
{
	// read from a file of recorded events and fire them at the right time

public:
	GamePlayer();
	~GamePlayer() { cleanup(); };

	void cleanup();
	void startPlayback();
	void stopPlayback();
	bool isPlaying() { return mPlaying; };
	void load();

private:
	CircularQueue<Event*>* mpEventPlayback;
	bool mPlaying;
};