#pragma once

#include <EventListener.h>
#include <fstream>
#include <InterfaceEvents.h>

using namespace std;

class GameRecorder : public EventListener
{
	// record InterfaceEvents and write them later for playback

public:
	GameRecorder();
	~GameRecorder() { cleanup(); };

	void startRecording();
	void stopRecording();
	bool isRecording() { return mRecording; };
	void write(const Event& theEvent);
	void cleanup();
	
	void handleEvent(const Event& theEvent);
	
	int getCount() { return mCount; };

private:
	bool mRecording;
	double mDeltaTime;
	int mCount;
	ofstream mTextOutput, mBinaryInput;
};