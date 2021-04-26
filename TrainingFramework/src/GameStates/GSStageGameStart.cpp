#include "GSStageGameStart.h"
#include <iostream>
#include <fstream>
extern int screenWidth; //need get on Graphic engine
extern int screenHeight; //need get on Graphic engine
using namespace std;

GSStageGameStart::GSStageGameStart()
{
	m_time = 0;
}


GSStageGameStart::~GSStageGameStart()
{
}

int GSStageGameStart::GetLevel() {
		std::ifstream fileInput("C:\\Users\\dell\\Desktop\\Programming_anim\\TrainingFramework\\src\\GameStates\\level.txt");

		if (fileInput.fail())
		{
			std::cout << "Failed to open this file!" << std::endl;
			return -1;
		}
		while (!fileInput.eof())
		{
			int n;
			fileInput >> n;
			std::cout << "n = " << n << "\n";
			level = n;
		}
		std::cout << std::endl;

		fileInput.close();
}

void GSStageGameStart::Init()
{
	GetLevel();
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("stage_game_start/stage"+to_string(level));

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
