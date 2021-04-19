#include "GSStageGameStart.h"


extern int screenWidth; //need get on Graphic engine
extern int screenHeight; //need get on Graphic engine


GSStageGameStart::GSStageGameStart()
{
	m_time = 0;
}


GSStageGameStart::~GSStageGameStart()
{
}


void GSStageGameStart::Init()
{

	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("stage_game_start/stage1");

	m_logo = std::make_shared<Sprite2D>(model, shader, texture);
	m_logo->Set2DPosition(screenWidth / 2, screenHeight / 2);
	m_logo->SetSize(screenWidth, screenHeight);
	ResourceManagers::GetInstance()->PlaySound("stage_game_start", false);

}

void GSStageGameStart::Exit()
{
	
}


void GSStageGameStart::Pause()
{
	
}

void GSStageGameStart::Resume()
{
	
}


void GSStageGameStart::HandleEvents()
{
	
}

void GSStageGameStart::HandleKeyEvents(int key, bool bIsPressed)
{

}

void GSStageGameStart::HandleTouchEvents(int x, int y, bool bIsPressed)
{

}

void GSStageGameStart::Update(float deltaTime)
{
	m_time += deltaTime;
	if (m_time > 2.75)
	{
		GameStateMachine::GetInstance()->ChangeState(StateTypes::STATE_Play);
		m_time = 0;
	}
}

void GSStageGameStart::Draw()
{
	m_logo->Draw();
}
