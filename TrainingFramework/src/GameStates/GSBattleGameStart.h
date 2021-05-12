#pragma once
#include "gamestatebase.h"
#include "SpriteAnimation.h"

class GSBattleGameStart :
	public GameStateBase
{
public:
	GSBattleGameStart();
	~GSBattleGameStart();

	void Init();
	void Exit();

	void Pause();
	void Resume();

	void HandleEvents();
	void HandleKeyEvents(int key, bool bIsPressed);
	void HandleTouchEvents(int x, int y, bool bIsPressed);
	void Update(float deltaTime);
	void Draw();
private:
	std::shared_ptr<SpriteAnimation> m_logo;
	float m_time;
};

