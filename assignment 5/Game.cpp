#include <stdio.h>
#include <assert.h>
#include <iostream>

#include <sstream>

#include "Game.h"
#include <System.h>
#include <GraphicsSystem.h>
#include <GraphicsBuffer.h>
#include <Font.h>
#include <FontManager.h>
#include <GraphicsBufferManager.h>
#include <InputSystem.h>
#include "GameMessageManager.h"
#include <Sprite.h>
#include "SpriteManager.h"
#include <Timer.h>
#include <DataRepository.h>
#include "PlayerMoveToMessage.h"
#include "CreateUnitMessage.h"
#include "ComponentManager.h"
#include "UnitManager.h"
#include "DataLoader.h"
#include "ExtraColors.h"

Game* gpGame = NULL;

Game::Game()
	:Trackable("game class")
	,mpSystem(NULL)
	,mpGraphicsBufferManager(NULL)
	,mpSpriteManager(NULL)
	,mpFontManager(NULL)
	,mpLoopTimer(NULL)
	,mpMasterTimer(NULL)
	,mpFont(NULL)
	,mShouldExit(false)
	,mBackgroundBufferID("")
	,mpMessageManager(NULL)
	,mpComponentManager(NULL)
	,mpUnitManager(NULL)
	,mpRepository(NULL)
	,mTimeLastFrame(0.0f)
{
}

Game::~Game()
{
	cleanup();
}

bool Game::init()
{
	mShouldExit = false;

	//create Timers
	mpLoopTimer = new Timer;
	mpMasterTimer = new Timer;

	mpRepository = new DataRepository;
	DataLoader loader("data.txt", mpRepository);

	//create and init GraphicsSystem
	mpSystem = new System();
	bool goodGraphics = mpSystem->init( mpRepository->getEntry(DataKeyEnum::SCREEN_WIDTH).getUIntVal(), mpRepository->getEntry(DataKeyEnum::SCREEN_HEIGHT).getUIntVal());
	if(!goodGraphics) 
	{
		fprintf(stderr, "failed to initialize GraphicsSystem object!\n");
		return false;
	}

	mpGraphicsBufferManager = new GraphicsBufferManager(mpSystem->getGraphicsSystem());
	mpSpriteManager = new SpriteManager;
	mpFontManager = new FontManager;

	mpMessageManager = new GameMessageManager();

	UINT maxUnits = mpRepository->getEntry(DataKeyEnum::MAX_UNITS).getUIntVal();
	mpComponentManager = new ComponentManager(maxUnits);
	mpUnitManager = new UnitManager(maxUnits);

	//load buffers
	GraphicsBuffer* pBuff;
	pBuff = mpGraphicsBufferManager->loadBuffer(mBackgroundBufferID,"wallpaper.bmp");
	pBuff = mpGraphicsBufferManager->loadBuffer(mPlayerIconBufferID,"arrow.png");
	pBuff = mpGraphicsBufferManager->loadBuffer(mEnemyIconBufferID, "enemy-arrow.png");
	pBuff = mpGraphicsBufferManager->loadBuffer(mTargetBufferID,"target.png");

	//load Font
	mpFont = mpFontManager->createAndManageFont(COUR_24_FONT_ID, "cour.ttf", 24);
	
	//setup sprites
	GraphicsBuffer* pBackGroundBuffer = mpGraphicsBufferManager->getBuffer( mBackgroundBufferID );
	if( pBackGroundBuffer != NULL )
	{

		mpSpriteManager->createAndManageSprite( BACKGROUND_SPRITE_ID, pBackGroundBuffer, 0, 0, (float)pBackGroundBuffer->getWidth(), (float)pBackGroundBuffer->getHeight() );
	}
	GraphicsBuffer* pPlayerBuffer = mpGraphicsBufferManager->getBuffer( mPlayerIconBufferID );
	Sprite* pArrowSprite = NULL;
	if( pPlayerBuffer != NULL )
	{
		pArrowSprite = mpSpriteManager->createAndManageSprite( PLAYER_ICON_SPRITE_ID, pPlayerBuffer, 0, 0, (float)pPlayerBuffer->getWidth(), (float)pPlayerBuffer->getHeight() );
	}
	GraphicsBuffer* pAIBuffer = mpGraphicsBufferManager->getBuffer(mEnemyIconBufferID);
	Sprite* pEnemyArrow = NULL;
	if (pAIBuffer != NULL)
	{
		pEnemyArrow = mpSpriteManager->createAndManageSprite(AI_ICON_SPRITE_ID, pAIBuffer, 0, 0, (float)pAIBuffer->getWidth(), (float)pAIBuffer->getHeight());
	}

	GraphicsBuffer* pTargetBuffer = mpGraphicsBufferManager->getBuffer(mTargetBufferID);
	if (pTargetBuffer != NULL)
	{
		mpSpriteManager->createAndManageSprite(TARGET_SPRITE_ID, pTargetBuffer, 0, 0, (float)pTargetBuffer->getWidth(), (float)pTargetBuffer->getHeight());
	}

	
	//setup units
	Unit* pUnit = NULL;

	if (pArrowSprite)
	{
		Vector2D center((int)getGraphicsSystem()->getDisplayWidth() / 2, (int)getGraphicsSystem()->getDisplayHeight() / 2);
		pUnit = mpUnitManager->createPlayerUnit(*pArrowSprite, false, PositionData(center, 0.0f));
		pUnit->setShowTarget(true);
		pUnit->setSteering(Steering::FACE, Vector2D((int)getGraphicsSystem()->getDisplayWidth() / 2, 700));
	}

	//create 2 enemies
	if (pEnemyArrow)
	{
		pUnit = mpUnitManager->createUnit(*pEnemyArrow, true, PositionData(Vector2D((float)GraphicsSystem::getDisplayWidth() - 1, 0.0f), 0.0f));
		pUnit->setShowTarget(true);
		pUnit->setSteering(Steering::SEEK, ZERO_VECTOR2D, PLAYER_UNIT_ID);

		pUnit = mpUnitManager->createUnit(*pEnemyArrow, true, PositionData(Vector2D(0.0f, (float)GraphicsSystem::getDisplayHeight() - 1 - 1), 0.0f));
		pUnit->setShowTarget(false);
		pUnit->setSteering(Steering::FLEE, ZERO_VECTOR2D, PLAYER_UNIT_ID);
	}

	mTargetFPS = mpRepository->getEntry(DataKeyEnum::TARGET_FPS).getUIntVal();
	mTargetElapsedTime = 1000.0f / mTargetFPS;

	mGamePlayer = new GamePlayer();
	mGameRecorder = new GameRecorder();

	mButtonByteSize = 0;
	mMoveByteSize = 0;
	mKeyboardByteSize = 0;

	return true;
}

void Game::cleanup()
{
	delete mGamePlayer;
	mGamePlayer = NULL;
	delete mGameRecorder;
	mGameRecorder = NULL;

	//delete the timers
	delete mpLoopTimer;
	mpLoopTimer = NULL;
	delete mpMasterTimer;
	mpMasterTimer = NULL;

	delete mpFontManager;
	mpFontManager = NULL;

	delete mpRepository;
	mpRepository = NULL;

	mpFont = NULL;

	//delete the graphics system
	delete mpSystem;
	mpSystem = NULL;

	delete mpGraphicsBufferManager;
	mpGraphicsBufferManager = NULL;
	delete mpSpriteManager;
	mpSpriteManager = NULL;
	delete mpMessageManager;
	mpMessageManager = NULL;
	delete mpUnitManager;
	mpUnitManager = NULL;
	delete mpComponentManager;
	mpComponentManager = NULL;

	mButtonByteSize = 0;
	mMoveByteSize = 0;
	mKeyboardByteSize = 0;
}

void Game::doLoop()
{
	//game loop
	while (!mShouldExit)
	{
		gpPerformanceTracker->clearTracker("loop");
		gpPerformanceTracker->startTracking("loop");

		gpGame->beginLoop();

		gpPerformanceTracker->clearTracker("process");
		gpPerformanceTracker->startTracking("process");

		gpGame->processLoop();

		gpPerformanceTracker->stopTracking("process");

		gpGame->endLoop();

		gpPerformanceTracker->stopTracking("loop");
		std::cout << "loop took:" << gpPerformanceTracker->getElapsedTime("loop") << "ms    ";
		std::cout << "processing took:" << gpPerformanceTracker->getElapsedTime("process") << "ms\n";
		mTimeLastFrame = (float)gpPerformanceTracker->getElapsedTime("loop");
	}
}

void Game::beginLoop()
{
	mpLoopTimer->start();
}

void Game::processLoop()
{
	InputSystem* pInputSystem = mpSystem->getInputSystem();

	float dt = (mTargetElapsedTime * mTimeMult) / 1000.0f;
	mpUnitManager->updateAll(dt);
	mpComponentManager->update(dt);
	pInputSystem->update(dt);
	
	//draw background
	//GraphicsSystem::renderFilledRect(*GraphicsSystem::getBackBuffer(), ZERO_VECTOR2D, GraphicsSystem::getDisplayWidth(), GraphicsSystem::getDisplayHeight(), BACKGROUND_BLUE_COLOR);

	Sprite* pBackgroundSprite = mpSpriteManager->getSprite(BACKGROUND_SPRITE_ID);
	GraphicsSystem::draw(Vector2D(0, 0), *pBackgroundSprite);

	//draw units
	mpUnitManager->drawAll();

	if (mDrawDebugData)
	{
		drawDebugData();
	}

	mpSystem->getGraphicsSystem()->flip();

	Vector2D pos = pInputSystem->getCurrentMousePos();

	mpMessageManager->processMessagesForThisframe();

	//read input
	if(pInputSystem->isMouseButtonPressed(InputSystem::LEFT))
	{
		GameMessage* pMessage = new PlayerMoveToMessage( pos );
		MESSAGE_MANAGER->addMessage( pMessage, 0 );
	}

	if(pInputSystem->isKeyPressed(InputSystem::ESCAPE_KEY))
	{
		mShouldExit = true;
	}

	if (pInputSystem->isKeyPressed(InputSystem::M_KEY))
	{
		for (int i = 0; i < 10; i++)
		{
			GameMessage* pMessage = new CreateUnitMessage;
			MESSAGE_MANAGER->addMessage(pMessage, i*250);
		}
	}

	if (pInputSystem->isKeyPressed(InputSystem::UP_KEY))
	{
		GameMessage* pMessage = new CreateUnitMessage;
		MESSAGE_MANAGER->addMessage(pMessage);
	}
	else if (pInputSystem->isKeyPressed(InputSystem::DOWN_KEY))
	{
		mpUnitManager->deleteRandomUnit();
	}

	if (pInputSystem->isKeyPressed(InputSystem::LEFT_KEY))
	{
		mTimeMult -= 0.1f;
		if (mTimeMult < 0.0f)
			mTimeMult = 0.0f;
	}
	else if (pInputSystem->isKeyPressed(InputSystem::RIGHT_KEY))
	{
		mTimeMult += 0.1f;
	}
	else if (pInputSystem->isKeyPressed(InputSystem::SPACE_KEY))
	{
		mTimeMult = 1.0f;
	}


	if (pInputSystem->isKeyPressed(InputSystem::D_KEY))
	{
		mDrawDebugData = true;
	}
	else
	{
		mDrawDebugData = false;
	}

	//assignment 5
	if (pInputSystem->isKeyPressed(InputSystem::R_KEY)) //start recording
	{
		if (mGamePlayer->isPlaying() == false && mGameRecorder->isRecording() == false)
		{
			EventSystem::getInstance()->addListener(Event::MOUSE_BUTTON_EVENT, mGameRecorder);
			EventSystem::getInstance()->addListener(Event::MOUSE_MOVE_EVENT, mGameRecorder);
			EventSystem::getInstance()->addListener(Event::KEYBOARD_EVENT, mGameRecorder);

			mGameRecorder->startRecording();
		}
	}

	if (pInputSystem->isKeyPressed(InputSystem::S_KEY)) //stop recording
	{
		if (mGameRecorder->isRecording())
		{
			EventSystem::getInstance()->removeListener(Event::MOUSE_BUTTON_EVENT, mGameRecorder);
			EventSystem::getInstance()->removeListener(Event::MOUSE_MOVE_EVENT, mGameRecorder);
			EventSystem::getInstance()->removeListener(Event::KEYBOARD_EVENT, mGameRecorder);

			mGameRecorder->stopRecording();
		}
	}

	if (pInputSystem->isKeyPressed(InputSystem::P_KEY)) //start playback
	{
		if (mGamePlayer->isPlaying() == false && mGameRecorder->isRecording() == false)
		{
			EventSystem::getInstance()->addListener(Event::MOUSE_BUTTON_EVENT, mGameRecorder);
			EventSystem::getInstance()->addListener(Event::MOUSE_MOVE_EVENT, mGameRecorder);
			EventSystem::getInstance()->addListener(Event::KEYBOARD_EVENT, mGameRecorder);

			mGamePlayer->load();
			mGamePlayer->startPlayback();
		}
	}

	if (pInputSystem->isKeyPressed(InputSystem::E_KEY)) //end playback
	{
		if (mGamePlayer->isPlaying())
		{
			EventSystem::getInstance()->removeListener(Event::MOUSE_BUTTON_EVENT, mGameRecorder);
			EventSystem::getInstance()->removeListener(Event::MOUSE_MOVE_EVENT, mGameRecorder);
			EventSystem::getInstance()->removeListener(Event::KEYBOARD_EVENT, mGameRecorder);

			mGamePlayer->stopPlayback();
		}
	}
}

void Game::endLoop()
{
	//mpMasterTimer->start();
	mpLoopTimer->sleepUntilElapsed( mTargetElapsedTime );
}

void Game::drawDebugData()
{
	InputSystem* pInputSystem = mpSystem->getInputSystem();

	Vector2D pos = pInputSystem->getCurrentMousePos();

	//create mouse text
	std::stringstream textStream;
	textStream << pos;

	//write text at mouse position
	GraphicsSystem::writeText(pos, *mpFont, BLACK_COLOR, textStream.str(), Font::RIGHT);

	textStream.str("");
	textStream.clear();

	//write out current number of Units
	Uint32 numUnits = mpUnitManager->getNumUnits();
	textStream << "Units:" << numUnits;
	GraphicsSystem::writeText(Vector2D(GraphicsSystem::getDisplayWidth(), 0), *mpFont, BLACK_COLOR, textStream.str(), Font::RIGHT);

	textStream.str("");
	textStream.clear();

	//write out current fps
	int fps = (int)((1000.0f / mTimeLastFrame) + .5f);//+.5f does rounding
	textStream << "FPS:" << fps;
	GraphicsSystem::writeText(Vector2D(GraphicsSystem::getDisplayWidth() / 2, 0), *mpFont, BLACK_COLOR, textStream.str(), Font::CENTER);

	textStream.str("");
	textStream.clear();

	//write out time multiple
	textStream << "time mult:" << mTimeMult;
	GraphicsSystem::writeText(Vector2D(GraphicsSystem::getDisplayWidth() / 4, 0), *mpFont, BLACK_COLOR, textStream.str(), Font::CENTER);

}

GraphicsSystem* Game::getGraphicsSystem() const
{
	return mpSystem->getGraphicsSystem();
}

void Game::handleEvent(const Event& theEvent)
{
	
}

float genRandomBinomial()
{
	return genRandomFloat() - genRandomFloat();
}

float genRandomFloat()
{
	float r = (float)rand()/(float)RAND_MAX;
	return r;
}

