#include "GSBattleResult.h"
#include <iostream>
#include <fstream>
extern int screenWidth; //need get on Graphic engine
extern int screenHeight; //need get on Graphic engine
using namespace std;

GSBattleResult::GSBattleResult()
{
	m_time = 0;
}


GSBattleResult::~GSBattleResult()
{
}

int GSBattleResult::GetResult() {
	std::ifstream fileInput("C:\\Users\\dell\\Desktop\\Programming_anim\\TrainingFramework\\src\\GameStates\\battle_result.txt");

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
		result = n;
	}
	std::cout << std::endl;

	fileInput.close();
}

void GSBattleResult::Init()
{
	GetResult();
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("battle_game_start/" + to_string(result));

	m_logo = std::make_shared<Sprite2D>(model, shader, texture);
	m_logo->Set2DPosition(screenWidth / 2, screenHeight / 2);
	m_logo->SetSize(screenHeight, screenHeight);
	ResourceManagers::GetInstance()->PlaySound(result == 0 ? "battle_draw" : "battle_win", false);
}

void GSBattleResult::Exit()
{

}


void GSBattleResult::Pause()
{

}

void GSBattleResult::Resume()
{

}


void GSBattleResult::HandleEvents()
{

}

void GSBattleResult::HandleKeyEvents(int key, bool bIsPressed)
{

}

void GSBattleResult::HandleTouchEvents(int x, int y, bool bIsPressed)
{

}

void GSBattleResult::Update(float deltaTime)
{
	m_time += deltaTime;
	if (m_time > 5)
	{
		ResourceManagers::GetInstance()->PlaySound("Level1/TitleScreen", true);
		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->PopState();
		m_time = 0;
	}
}

void GSBattleResult::Draw()
{
	m_logo->Draw();
}
