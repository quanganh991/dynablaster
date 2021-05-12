/*
#include "GSIntro.h"
#include "GSMenu.h"
*/
#include "GSPlay.h"
#include "GSBattle.h"
#include "GSIntro.h"
#include "GSMenu.h"
#include "GSStageGameStart.h"
#include "GameStatebase.h"
#include "GSBattleGameStart.h"
#include "GSBattleResult.h"

std::shared_ptr<GameStateBase> GameStateBase::CreateState(StateTypes stt)
{
	std::shared_ptr<GameStateBase> gs = nullptr;
	switch (stt)
	{
	case STATE_INVALID:
		break;
	case STATE_Intro:
		gs = std::make_shared<GSIntro>();
		break;
	case STATE_Menu:
		gs = std::make_shared<GSMenu>();
		break;
	case STATE_Play:
		gs = std::make_shared<GSPlay>();
		break;
	case STATE_StageGameStart:
		gs = std::make_shared<GSStageGameStart>();
		break;
	case STATE_Battle:
		gs = std::make_shared<GSBattle>();
		break;
	case STATE_BattleGameStart:
		gs = std::make_shared<GSBattleGameStart>();
		break;
	case STATE_BattleResult:
		gs = std::make_shared<GSBattleResult>();
		break;
	default:
		break;
	}
	return gs;
}