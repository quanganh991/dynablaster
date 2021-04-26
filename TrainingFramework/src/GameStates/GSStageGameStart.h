#pragma once
#include "gamestatebase.h"

class GSStageGameStart :
	public GameStateBase
{
public:
	GSStageGameStart();
	~GSStageGameStart();

	void Init();
	void Exit();

	void Pause();
	void Resume();

	void HandleEvents();
	void HandleKeyEvents(int key, bool bIsPressed);
	void HandleTouchEvents(int x, int y, bool bIsPressed);
	void Update(float deltaTime);
	void Draw();
	int GetLevel();
private:
	int level = 1;
	std::shared_ptr<Sprite2D> m_logo;
	float m_time;
};

