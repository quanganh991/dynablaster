#pragma once
#include "gamestatebase.h"
#include "GameButton.h"
#include "SpriteAnimation.h"
#include <math.h>
#include <queue>
#define Pi 3.141592653589793
#include <windows.h>
#include<iostream>
using namespace std;
class Sprite2D;
class Sprite3D;
class Text;

typedef struct BOMB {
	int index;	//1,2
	int xBlock;
	int yBlock;
} BOMB;

class GSPlay :
	public GameStateBase
{
public:
	GSPlay();
	~GSPlay();

	void Init();
	void InitBricks();
	void InitRocks();
	void InitEnemies();
	void Exit();

	void Pause();
	void Resume();

	int getWidthBlock_from_WidthPixel(int screenWidth);
	int getHeightBlock_from_HeightPixel(int screenHeight);
	int getWidthPixel_from_WidthBlock(int now_WidthBlock);
	int getHeightPixel_from_HeightBlock(int now_HeightBlock);

	bool isGrass(int width, int height);	//kiểm tra xem có phải cỏ ko

	void SetBomb(int pixelWidth, int pixelHeight);	//đặt bom
	void SetExplode(int pixelWidth, int pixelHeight);	//nổ bom+
	void FinishBomb(float deltaTime);	//nổ bom-
	void FinishExplode(float deltaTime);	//bom cháy hết
	
	void SetBomb2(int pixelWidth, int pixelHeight);	//đặt bom
	void SetExplode2(int pixelWidth, int pixelHeight);	//nổ bom+
	void FinishBomb2(float deltaTime);	//nổ bom-
	void FinishExplode2(float deltaTime);	//bom cháy hết

	void HasBombermanBeenFired(float deltaTime);
	void HasEnemiesBeenFired(float deltaTime);
	void HasBombermanBeenTouchedByEnemies(float deltaTime);

	void ExplodeLeft(int pixelWidth, int pixelHeight, int whichFire);
	void ExplodeRight(int pixelWidth, int pixelHeight, int whichFire);
	void ExplodeUp(int pixelWidth, int pixelHeight, int whichFire);
	void ExplodeDown(int pixelWidth, int pixelHeight, int whichFire);

	void EnemiesMoving(float deltaTime);
	void EnemiesChangeDirection(float deltaTime);

	void HandleEvents();
	void HandleKeyEvents(int key, bool bIsPressed);

	void HandleTouchEvents(int x, int y, bool bIsPressed);
	void Update(float deltaTime);
	void Draw();
	void DiedBomberman(float deltaTime);
	void PlayAgain(float deltaTime);
private:

	std::vector<std::shared_ptr<SpriteAnimation>> m_bombs;
	std::vector<std::shared_ptr<Sprite2D>> m_fires;
	std::vector<std::shared_ptr<SpriteAnimation>> m_bombs2;	//chứa bom2
	std::vector<std::shared_ptr<Sprite2D>> m_fires2;	//chứa tia lửa của bom2

	std::vector<std::shared_ptr<Sprite2D>> m_bricks;
	std::vector<std::shared_ptr<Sprite2D>> m_rocks;

	std::vector<std::shared_ptr<SpriteAnimation>> m_enemies;
	std::vector<int> m_enemies_direction;	//hướng di chuyển của enemies, 1 trái, 2 phải, 3 lên, 4 xuống
	std::vector<int> Venemies;	//Vận tốc của enemies

	std::shared_ptr<Sprite2D> m_skull;
	std::shared_ptr<Sprite2D> m_BackGround;
	std::shared_ptr<Text>  m_score;
	std::shared_ptr<Text>  m_coins;
	std::shared_ptr<Text>  m_level;
	std::vector<std::shared_ptr<GameButton>>	m_listButton;
	std::vector<std::shared_ptr<SpriteAnimation>> m_listSpriteAnimations;
	std::shared_ptr<SpriteAnimation> buttonDragDrop;
	
	float finishFireTime = 1; //thời gian tia lửa của bom 1 tồn tại là 1s
	bool hasFireBeenFinished = 1;

	float finishFire2Time = 1; //thời gian tia lửa của bom 2 tồn tại là 1s
	bool hasFire2BeenFinished = 1;


	float finishBomb1Time = 4;	//thời gian để bom1 nổ là 4s
	bool hasBomb1BeenFinished = 1;

	float finishBomb2Time = 4;	//thời gian để bom2 nổ là 4s
	bool hasBomb2BeenFinished = 1;

	//int MAXBOMB = 2;
	int level = 1;	//level cũng chính là số bom tối đa cho mỗi lần đặt
	queue<BOMB> bombstatus;
	double m_time = 0;
};