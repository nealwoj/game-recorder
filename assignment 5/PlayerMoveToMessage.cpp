#include "Game.h"
#include "GameMessageManager.h"
#include "PlayerMoveToMessage.h"
#include "CreateUnitMessage.h"
#include "UnitManager.h"

PlayerMoveToMessage::PlayerMoveToMessage( const Vector2D& pos )
:GameMessage(PLAYER_MOVETO_MESSAGE)
,mPos(pos)
{
}

PlayerMoveToMessage::~PlayerMoveToMessage()
{
}

void PlayerMoveToMessage::process()
{
	Unit* pPlayer = gpGame->getUnitManager()->getPlayerUnit();
	if (pPlayer)
	{
		pPlayer->setSteering(Steering::FACE, mPos);
	}
	GameMessage* pMessage = new CreateUnitMessage;
	MESSAGE_MANAGER->addMessage(pMessage);
}

