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

typedef struct BattleBomb {
	int index;	//1,2, 1 là bomb được đặt bằng phím cách, 2 là bomb được đặt bằng phím shift
	int xBlock;
	int yBlock;
} BattleBomb;

class GSBattle :
	public GameStateBase
{
public:
	GSBattle();
	~GSBattle();

	void Init();
	void InitBricks();
	void InitRocks();
	void InitShoes();

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

	void ExplodeLeft(int pixelWidth, int pixelHeight, int whichFire);
	void ExplodeRight(int pixelWidth, int pixelHeight, int whichFire);
	void ExplodeUp(int pixelWidth, int pixelHeight, int whichFire);
	void ExplodeDown(int pixelWidth, int pixelHeight, int whichFire);


	void HandleEvents();
	void HandleKeyEvents(int key, bool bIsPressed);

	void HandleTouchEvents(int x, int y, bool bIsPressed);
	void Update(float deltaTime);
	void Draw();
	void DiedBomberman(float deltaTime);
	void DiedBomberman2(float deltaTime);
	void ShowResult(float deltaTime);
	void CheckBreakShoes(); void eatShoes();
private:

	std::vector<std::shared_ptr<SpriteAnimation>> m_bombs;
	std::vector<std::shared_ptr<SpriteAnimation>> m_fires;
	std::vector<std::shared_ptr<SpriteAnimation>> m_bombs2;	//chứa bom2
	std::vector<std::shared_ptr<SpriteAnimation>> m_fires2;	//chứa tia lửa của bom2

	std::vector<std::shared_ptr<Sprite2D>> m_bricks;
	std::vector<std::shared_ptr<Sprite2D>> m_rocks;
	std::vector<std::shared_ptr<Sprite2D>> m_borders;
	std::shared_ptr<Sprite2D> m_shoes;

	std::shared_ptr<Sprite2D> m_skull;
	std::shared_ptr<Sprite2D> btnLeft;
	std::shared_ptr<Sprite2D> btnRight;
	std::shared_ptr<Sprite2D> btnUp;
	std::shared_ptr<Sprite2D> btnDown;
	std::shared_ptr<Sprite2D> btnSetBomb;
	std::shared_ptr<Sprite2D> m_BackGround;
	std::shared_ptr<Text>  m_score;
	std::shared_ptr<Text>  m_coins;
	std::vector<std::shared_ptr<GameButton>>	m_listButton;
	std::vector<std::shared_ptr<SpriteAnimation>> m_listSpriteAnimations;
	std::shared_ptr<SpriteAnimation> Player1;
	std::shared_ptr<SpriteAnimation> Player2;


	float finishFireTime = 1; //thời gian tia lửa của bom 1 tồn tại là 1s
	bool hasFireBeenFinished = 1;

	float finishFire2Time = 1; //thời gian tia lửa của bom 2 tồn tại là 1s
	bool hasFire2BeenFinished = 1;


	float finishBomb1Time = 4;	//thời gian để bom1 nổ là 4s
	bool hasBomb1BeenFinished = 1;

	float finishBomb2Time = 4;	//thời gian để bom2 nổ là 4s
	bool hasBomb2BeenFinished = 1;

	int shoesWidthByBlock;	//Vị trí của giày theo chiều ngang
	int shoesHeightByBlock;	//Vị trí của giày theo chiều dọc
	int HasShoesBeenExplored = 0;	//Kiểm tra xem bomberman đã tìm được giày chưa
	queue<BattleBomb> bombstatus;
	double m_time = 0;
	int delta_pixel_1 = 10;
	int delta_pixel_2 = 50;
	int deaths = 0;
};