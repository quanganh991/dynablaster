#include "GSBattleGameStart.h"
#include <iostream>
#include <fstream>
extern int screenWidth; //need get on Graphic engine
extern int screenHeight; //need get on Graphic engine
using namespace std;

GSBattleGameStart::GSBattleGameStart()
{
	m_time = 0;
}


GSBattleGameStart::~GSBattleGameStart()
{
}

void GSBattleGameStart::Init()
{
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
	auto texture = ResourceManagers::GetInstance()->GetTexture("battle_game_start/battle_start");

	m_logo = std::make_shared<SpriteAnimation>(model, shader, texture, 2, 1.0f/4);
	m_logo->Set2DPosition(screenWidth / 2, screenHeight / 2);
	m_logo->SetSize(screenHeight, screenHeight);
	ResourceManagers::GetInstance()->PlaySound("battle_start", false);

}

void GSBattleGameStart::Exit()
{

}


void GSBattleGameStart::Pause()
{

}

void GSBattleGameStart::Resume()
{

}


void GSBattleGameStart::HandleEvents()
{

}

void GSBattleGameStart::HandleKeyEvents(int key, bool bIsPressed)
{

}

void GSBattleGameStart::HandleTouchEvents(int x, int y, bool bIsPressed)
{

}

void GSBattleGameStart::Update(float deltaTime)
{
	m_logo->Update(deltaTime);
	m_time += deltaTime;
	if (m_time > 6)
	{
		GameStateMachine::GetInstance()->ChangeState(StateTypes::STATE_Battle);
		m_time = 0;
	}
}

void GSBattleGameStart::Draw()
{
	m_logo->Draw();
}
