#include "GSBattle.h"
#include "Shaders.h"
#include "Texture.h"
#include "Models.h"
#include "Camera.h"
#include "Font.h"
#include "Sprite2D.h"
#include "Sprite3D.h"
#include "Text.h"
#include "iostream"
#include "SpriteAnimation.h"
#include "fstream"
#define MAX_RD_DELTATIME 90
#define MIN_RD_DELTATIME 80
#define INITIAL_SPEED 6
#define OMEGA 100
using namespace std;
extern int screenWidth; //need get on Graphic engine
extern int screenHeight; //need get on Graphic engine

int RealScreenWidth = screenWidth * 1350 / 1520; // = 1350 pixel
int RealScreenHeight = screenHeight * 650 / 800; // = 650 pixel

GSBattle::GSBattle()
{
}


GSBattle::~GSBattle()
{

}

//1. Chuyển đổi pixel sang các ô và ngược lại

//Chiều ngang: 1350 pixel, 50 pixel/block, 27 block = 14 gạch/cỏ + 13 đá
int GSBattle::getWidthBlock_from_WidthPixel(int now_WidthPixel) {
	if (0.5 * (screenWidth - RealScreenWidth) <= now_WidthPixel &&
		now_WidthPixel < RealScreenWidth + 0.5 * (screenWidth - RealScreenWidth))//từ 85 đến 1435
		return (int)((now_WidthPixel - 0.5 * (screenWidth - RealScreenWidth))	//thụt 85
			*27.0 / 1350 + 1.0);
	else if (now_WidthPixel == RealScreenWidth + 0.5 * (screenWidth - RealScreenWidth))
		return 27;
	else return 0;
}

//chiều cao: 650 pixel, 50 pixel/block, 13 block = 7 gạch/ cỏ + 6 đá
int GSBattle::getHeightBlock_from_HeightPixel(int now_HeightPixel) {
	if (0.5 * (screenHeight - RealScreenHeight) <= now_HeightPixel &&
		now_HeightPixel < RealScreenHeight + 0.5 * (screenHeight - RealScreenHeight)) //từ 75 đến 725
		return (int)((now_HeightPixel - 0.5 * (screenHeight - RealScreenHeight)) //thụt 75
			*13.0 / 650 + 1.0);
	else if (now_HeightPixel == RealScreenHeight + 0.5 * (screenHeight - RealScreenHeight))
		return 13;
	else return 0;
}

int GSBattle::getWidthPixel_from_WidthBlock(int now_WidthBlock) {	//chiều ngang
	return 50 * (now_WidthBlock*1.0 - 0.5f)*1.0f
		+ 0.5 * (screenWidth - RealScreenWidth);
}

int GSBattle::getHeightPixel_from_HeightBlock(int now_HeightBlock) {//chiều cao
	return 50 * (now_HeightBlock*1.0 - 0.5f)*1.0f
		+ 0.5 * (screenHeight - RealScreenHeight);
}

//2. đặt các viên gạch lên màn hình
void GSBattle::InitBricks() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/brick");
	for (int i = 1; i <= 27; i += 1) {	//width
		for (int j = 2; j <= 12; j += 1) {	//height
			if ((i % 2 != 0 || j % 2 != 0) && rand() % 2 == 1 && ((i != 26) || ((j != 11) && (j != 13))) && ((j != 12) || ((i != 25) && (i != 27)))) {
				int width_pixel = getWidthPixel_from_WidthBlock(i);	//lấy tâm pixel của block
				int height_pixel = getHeightPixel_from_HeightBlock(j);	//lấy tâm pixel của block
				std::shared_ptr<Sprite2D> brick = std::make_shared<Sprite2D>(model, shader, texture);
				brick->Set2DPosition(width_pixel, height_pixel);
				brick->SetSize(50, 50);
				m_bricks.push_back(brick);
			}
		}
	}


	int shoesPosition = rand() % m_bricks.size();
	shoesWidthByBlock = getWidthBlock_from_WidthPixel(m_bricks[shoesPosition]->Get2DPosition().x);	//lưu tọa độ theo block của giày
	shoesHeightByBlock = getHeightBlock_from_HeightPixel(m_bricks[shoesPosition]->Get2DPosition().y);
}

//3. đặt các viên đá lên màn hình
void GSBattle::InitRocks() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/Rock");
	for (int i = 2; i <= 26; i += 2) {	//width
		for (int j = 2; j <= 12; j += 2) {	//height
			int width_pixel = getWidthPixel_from_WidthBlock(i);	//lấy tâm pixel của block
			int height_pixel = getHeightPixel_from_HeightBlock(j);	//lấy tâm pixel của block
			std::shared_ptr<Sprite2D> rock = std::make_shared<Sprite2D>(model, shader, texture);
			rock->Set2DPosition(width_pixel, height_pixel);
			rock->SetSize(50, 50);
			m_rocks.push_back(rock);
		}
	}

	for (int i = 1; i <= 27; i++) {	//border chiều ngang, phần dưới
		int width_pixel = getWidthPixel_from_WidthBlock(i);
		int height_pixel = getHeightPixel_from_HeightBlock(14);
		std::shared_ptr<Sprite2D> border = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/border"));
		border->Set2DPosition(width_pixel, height_pixel);
		border->SetSize(50, 50);
		m_borders.push_back(border);
	}
	for (int i = 1; i <= 14; i++) {	//border chiều dọc, bên trái
		int width_pixel = getWidthPixel_from_WidthBlock(0);
		int height_pixel = getHeightPixel_from_HeightBlock(i);
		std::shared_ptr<Sprite2D> border = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/border"));
		border->Set2DPosition(width_pixel, height_pixel);
		border->SetSize(50, 50);
		m_borders.push_back(border);
	}
	for (int i = 1; i <= 14; i++) {	//border chiều dọc, bên phải
		int width_pixel = getWidthPixel_from_WidthBlock(28);
		int height_pixel = getHeightPixel_from_HeightBlock(i);
		std::shared_ptr<Sprite2D> border = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/border"));
		border->Set2DPosition(width_pixel, height_pixel);
		border->SetSize(50, 50);
		m_borders.push_back(border);
	}
}


//4. đặt các Shoes lên màn hình
void GSBattle::InitShoes() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/shoes");
	m_shoes = std::make_shared<Sprite2D>(model, shader, texture);
	m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(200), getHeightPixel_from_HeightBlock(200));	//Giày khởi tạo ở vô cực
	m_shoes->SetSize(50, 100);
}

//5. tương đương với constructor khởi tạo trong java
void GSBattle::Init()
{
	InitBricks();
	InitRocks();
	InitShoes();

	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/background_battle");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	btnLeft = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/left"));
	btnLeft->Set2DPosition(getWidthPixel_from_WidthBlock(25), getWidthPixel_from_WidthBlock(12) - 10);
	btnLeft->SetSize(40, 40);

	btnRight = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/right"));
	btnRight->Set2DPosition(getWidthPixel_from_WidthBlock(27), getWidthPixel_from_WidthBlock(12) - 10);
	btnRight->SetSize(40, 40);

	btnUp = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/up"));
	btnUp->Set2DPosition(getWidthPixel_from_WidthBlock(26), getWidthPixel_from_WidthBlock(11) - 10);
	btnUp->SetSize(40, 40);

	btnDown = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/down"));
	btnDown->Set2DPosition(getWidthPixel_from_WidthBlock(26), getWidthPixel_from_WidthBlock(13) - 10);
	btnDown->SetSize(40, 40);

	btnSetBomb = std::make_shared<Sprite2D>(model, shader, ResourceManagers::GetInstance()->GetTexture("bomberman/setbomb"));
	btnSetBomb->Set2DPosition(getWidthPixel_from_WidthBlock(26), getWidthPixel_from_WidthBlock(12) - 10);
	btnSetBomb->SetSize(70, 70);
	
	m_BackGround = std::make_shared<Sprite2D>(model, shader, texture);
	m_BackGround->Set2DPosition(screenWidth / 2, screenHeight / 2);
	m_BackGround->SetSize(RealScreenWidth, RealScreenHeight);

	//Set vị trí skull nằm ngoài màn hình
	m_skull = std::make_shared<Sprite2D>(
		ResourceManagers::GetInstance()->GetModel("Sprite2D"),
		ResourceManagers::GetInstance()->GetShader("TextureShader"),
		ResourceManagers::GetInstance()->GetTexture("bomberman/bomberman_dead_2")
		);
	m_skull->Set2DPosition(getWidthPixel_from_WidthBlock((100)), getHeightPixel_from_HeightBlock((100)));
	m_skull->SetSize(50, 50);
	//

	//nút back
	texture = ResourceManagers::GetInstance()->GetTexture("button_back");
	std::shared_ptr<GameButton> button = std::make_shared<GameButton>(model, shader, texture);
	button->Set2DPosition(screenWidth - 75, 30);
	button->SetSize(150, 50);
	button->SetOnClick([]() {
		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->PopState();
		ResourceManagers::GetInstance()->PlaySound("Level1/TitleScreen", true);
	});
	m_listButton.push_back(button);
	//m_listButton.erase(m_listButton.begin() + chỉ số vector cần xóa);

	//bomberman
	shader = ResourceManagers::GetInstance()->GetShader("Animation");
	texture = ResourceManagers::GetInstance()->GetTexture("bomberman/bomberman_running");
	Player1 = std::make_shared<SpriteAnimation>(model, shader, texture, 12, 1.0f / 30);
	Player1->Set2DPosition(
		getWidthPixel_from_WidthBlock(1),
		getHeightPixel_from_HeightBlock(1)
	);
	Player1->SetSize(50, 50);
	//bomberman2
	texture = ResourceManagers::GetInstance()->GetTexture("bomberman/bomberman_running-2");
	Player2 = std::make_shared<SpriteAnimation>(model, shader, texture, 12, 1.0f / 30);
	Player2->Set2DPosition(
		getWidthPixel_from_WidthBlock(27),
		getHeightPixel_from_HeightBlock(13)
	);
	Player2->SetSize(50, 50);

	shader = ResourceManagers::GetInstance()->GetShader("TextShader");
	std::shared_ptr<Font> font = ResourceManagers::GetInstance()->GetFont("arialbd");
	//score
	m_score = std::make_shared< Text>(shader, font, "Score: 0", TEXT_COLOR::RED, 1.0);	//so diem
	m_score->Set2DPosition(getWidthPixel_from_WidthBlock(0) - 25, getHeightPixel_from_HeightBlock(0));

	//coins total quantity
	m_coins = std::make_shared< Text>(shader, font, "x : 0", TEXT_COLOR::PURPLE, 1.0);	//so xu da an duoc
	m_coins->Set2DPosition(getWidthPixel_from_WidthBlock(6), getHeightPixel_from_HeightBlock(0));

	// Animation đồng xu
	shader = ResourceManagers::GetInstance()->GetShader("Animation");
	texture = ResourceManagers::GetInstance()->GetTexture("bomberman/bomberman_dead");
	std::shared_ptr<SpriteAnimation> objCoin = std::make_shared<SpriteAnimation>(model, shader, texture, 8, 1.0f / 4);
	objCoin->Set2DPosition(getWidthPixel_from_WidthBlock(5) + 20, getHeightPixel_from_HeightBlock(0) - 8);
	objCoin->SetSize(22, 22);
	m_listSpriteAnimations.push_back(objCoin);

	ResourceManagers::GetInstance()->PlaySound("Level1/BattleMusic", true);	//true là có lặp lại
}

void GSBattle::Exit()
{

}


void GSBattle::Pause()
{

}

void GSBattle::Resume()
{

}


void GSBattle::HandleEvents()
{

}

//6. hàm check bomb đã nổ chưa
void GSBattle::FinishBomb(float deltaTime) {
	if (finishBomb1Time > 0)
		finishBomb1Time -= 1 * deltaTime;
	//cout << "finishBomb1Time = " << finishBomb1Time << " hasBomb1BeenFinished = " << hasBomb1BeenFinished << "\n";
	if (finishBomb1Time < 0 && hasBomb1BeenFinished == 0) {	//nếu hết thời gian mà bom chưa nổ thì kick nổ quả bom
		ResourceManagers::GetInstance()->PlaySound("exploded", false);
		SetExplode(m_bombs[0]->Get2DPosition().x, m_bombs[0]->Get2DPosition().y);
		m_bombs.erase(m_bombs.begin() + 0);
		hasBomb1BeenFinished = 1;	// xem như bom 1 chưa nổ
	}

}

//7. bom nổ xong thì xóa hết fire trên màn hình
void GSBattle::FinishExplode(float deltaTime) {
	//bom 1
	if (finishFireTime > 0)	//nếu tia lửa vẫn còn xuất hiện
		finishFireTime -= 1 * deltaTime;
	if (finishFireTime < 0 && hasFireBeenFinished == 0) {	//nếu tia lửa đã cháy hết thì ẩn đi
		finishFireTime = 0;
		m_fires.clear();
		hasFireBeenFinished = 1;
	}

}

//8. được gọi khi người dùng click đặt bom
void GSBattle::SetBomb(int pixelWidth, int pixelHeight) {	//đặt bomb bằng phím space
	BattleBomb b; b.index = 1; b.xBlock = getWidthBlock_from_WidthPixel(pixelWidth); b.yBlock = getHeightBlock_from_HeightPixel(pixelHeight);
	bombstatus.push(b);
	int blockWidth = getWidthBlock_from_WidthPixel(pixelWidth);
	int blockHeight = getHeightBlock_from_HeightPixel(pixelHeight);
	if (hasBomb1BeenFinished == 1) {
		finishBomb1Time = 4;
		auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
		auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
		auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/bomb");
		std::shared_ptr<SpriteAnimation> bomb = std::make_shared<SpriteAnimation>(model, shader, texture, 2, 1.0f / 30);
		bomb->Set2DPosition(
			getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(pixelWidth)),
			getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(pixelHeight)));
		bomb->SetSize(75, 75);
		m_bombs.push_back(bomb);
		hasBomb1BeenFinished = 0;
	}
}

//9. Set các fire lên màn hình
void GSBattle::SetExplode(int pixelWidth, int pixelHeight) {	//khi bom 1 nổ thì gọi hàm này
	int blockWidth = getWidthBlock_from_WidthPixel(pixelWidth);
	int blockHeight = getHeightBlock_from_HeightPixel(pixelHeight);
	if (hasFireBeenFinished == 1) {
		finishFireTime = 1;
		finishBomb1Time = 4;
		//nổ về 4 phía
		ExplodeLeft(pixelWidth, pixelHeight, 1);
		ExplodeRight(pixelWidth, pixelHeight, 1);
		ExplodeUp(pixelWidth, pixelHeight, 1);
		ExplodeDown(pixelWidth, pixelHeight, 1);
		//bom 1 nổ, xem xét vị trí của nó so với bom 2 và trạng thái của bombstatus
		if (bombstatus.size() == 2) {	//nếu đang có 2 quả bom trên màn hình
			if (bombstatus.front().index == 1) {	//giả sử bom 1 nổ trước, tác động đến bom 2
				if (bombstatus.front().xBlock == bombstatus.back().xBlock) {	//cùng 1 cột
					if (abs(bombstatus.front().yBlock - bombstatus.back().yBlock) <= 8) {
						//cùng 1 hàng và cách nhau chưa tới 8 theo chiều ngang
						finishBomb2Time = 0.0000001;
					}
				}
				if (bombstatus.front().yBlock == bombstatus.back().yBlock) {	//cùng 1 hàng
					if (abs(bombstatus.front().xBlock - bombstatus.back().xBlock) <= 8) {
						//cùng 1 cột và cách nhau chưa tới 8 theo chiều dọc
						finishBomb2Time = 0.0000001;
					}
				}
			}
		}
		//
	}
	hasFireBeenFinished = 0;
	//MAXBattleBomb++;
	bombstatus.pop();
}


void GSBattle::ExplodeLeft(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 1; i <= 8; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth - 50 * i, pixelHeight) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth - 50 * i) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					if (blockWidths[j] == shoesWidthByBlock && blockHeights[j] == shoesHeightByBlock)
						CheckBreakShoes();
					//
					break;	//phá xong thì break
				}
			}
			//gặp gạch thì phá gạch và break luôn
			//ko gặp viên gạch nào thì có nghĩa là gặp đá, cũng break luôn
			break;
		}
		//gặp cỏ thì cho đi tiếp
		auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
		auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
		auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/fire");
		std::shared_ptr<SpriteAnimation> fireL = std::make_shared<SpriteAnimation>(model, shader, texture, 13, 1.0f / 13);


		int ROUNDED_widthPixel = getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(pixelWidth - 50 * i));
		int ROUNDED_heightPixel = getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(pixelHeight));

		if (getWidthBlock_from_WidthPixel(pixelWidth - 50 * i) > 0 && getHeightBlock_from_HeightPixel(pixelHeight) > 0) {
			fireL->Set2DPosition(
				ROUNDED_widthPixel,	//di chuyển tia lửa sang phải
				ROUNDED_heightPixel);
		}
		else fireL->Set2DPosition(0, 0);


		fireL->SetSize(50, 50);
		if (whichFire == 1)
			m_fires.push_back(fireL);
		else if (whichFire == 2)
			m_fires2.push_back(fireL);
	}
};
void GSBattle::ExplodeRight(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 1; i <= 8; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth + 50 * i, pixelHeight) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth + 50 * i) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == shoesWidthByBlock && blockHeights[j] == shoesHeightByBlock)
						CheckBreakShoes();
					//
					break;	//phá xong thì break
				}
			}
			//gặp gạch thì phá gạch và break luôn
			//ko gặp viên gạch nào thì có nghĩa là gặp đá, cũng break luôn
			break;
		}
		//gặp cỏ thì cho đi tiếp
		auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
		auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
		auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/fire");
		std::shared_ptr<SpriteAnimation> fireR = std::make_shared<SpriteAnimation>(model, shader, texture, 13, 1.0f / 13);


		int ROUNDED_widthPixel = getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(pixelWidth + 50 * i));
		int ROUNDED_heightPixel = getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(pixelHeight));

		if (getWidthBlock_from_WidthPixel(pixelWidth + 50 * i) > 0 && getHeightBlock_from_HeightPixel(pixelHeight) > 0) {
			fireR->Set2DPosition(
				ROUNDED_widthPixel,	//di chuyển tia lửa sang phải
				ROUNDED_heightPixel);
		}
		else fireR->Set2DPosition(0, 0);
		fireR->SetSize(50, 50);



		if (whichFire == 1)
			m_fires.push_back(fireR);
		else if (whichFire == 2)
			m_fires2.push_back(fireR);
	}
};
void GSBattle::ExplodeUp(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 1; i <= 8; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth, pixelHeight - 50 * i) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight - 50 * i) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == shoesWidthByBlock && blockHeights[j] == shoesHeightByBlock)
						CheckBreakShoes();
					//
					break;	//phá xong thì break
				}
			}
			//gặp gạch thì phá gạch và break luôn
			//ko gặp viên gạch nào thì có nghĩa là gặp đá, cũng break luôn
			break;
		}
		//gặp cỏ thì cho đi tiếp
		auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
		auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
		auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/fire");
		std::shared_ptr<SpriteAnimation> fireU = std::make_shared<SpriteAnimation>(model, shader, texture, 13, 1.0f / 13);

		int ROUNDED_widthPixel = getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(pixelWidth));
		int ROUNDED_heightPixel = getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(pixelHeight - 50 * i));

		if (getWidthBlock_from_WidthPixel(pixelWidth) > 0 && getHeightBlock_from_HeightPixel(pixelHeight - 50 * i) > 0) {
			fireU->Set2DPosition(
				ROUNDED_widthPixel,	//di chuyển tia lửa sang phải
				ROUNDED_heightPixel);
		}
		else fireU->Set2DPosition(0, 0);


		fireU->SetSize(50, 50);
		if (whichFire == 1)
			m_fires.push_back(fireU);
		else if (whichFire == 2)
			m_fires2.push_back(fireU);
	}
};
void GSBattle::ExplodeDown(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 1; i <= 8; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth, pixelHeight + 50 * i) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight + 50 * i) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == shoesWidthByBlock && blockHeights[j] == shoesHeightByBlock)
						CheckBreakShoes();
					//
					break;	//phá xong thì break
				}
			}
			//gặp gạch thì phá gạch và break luôn
			//ko gặp viên gạch nào thì có nghĩa là gặp đá, cũng break luôn
			break;
		}
		//gặp cỏ thì cho đi tiếp

		auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
		auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
		auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/fire");
		std::shared_ptr<SpriteAnimation> fireD = std::make_shared<SpriteAnimation>(model, shader, texture, 13, 1.0f / 13);

		int ROUNDED_widthPixel = getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(pixelWidth));
		int ROUNDED_heightPixel = getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(pixelHeight + 50 * i));

		if (getWidthBlock_from_WidthPixel(pixelWidth) > 0 && getHeightBlock_from_HeightPixel(pixelHeight + 50 * i) > 0) {
			fireD->Set2DPosition(
				ROUNDED_widthPixel,	//di chuyển tia lửa sang phải
				ROUNDED_heightPixel);
		}
		else fireD->Set2DPosition(0, 0);
		fireD->SetSize(50, 50);


		if (whichFire == 1)
			m_fires.push_back(fireD);
		else if (whichFire == 2)
			m_fires2.push_back(fireD);
	}
};

//10. kiểm tra xem nhân vật có được đi vào ô có tọa độ pixelWidth, pixelHeight hay ko
bool GSBattle::isGrass(int pixelWidth, int pixelHeight) {
	if (getWidthBlock_from_WidthPixel(pixelWidth) % 2 == 0 && getHeightBlock_from_HeightPixel(pixelHeight) % 2 == 0) {
		//cả 2 tọa độ của block đều chia hết cho 2 thì nó là đá
		return false;
	}
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		if (getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x) == getWidthBlock_from_WidthPixel(pixelWidth)
			&&
			getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y) == getHeightBlock_from_HeightPixel(pixelHeight))
			return false;
	}
	if (getWidthBlock_from_WidthPixel(pixelWidth) > 27 || getWidthBlock_from_WidthPixel(pixelWidth) < 1) return false;
	if (getHeightBlock_from_HeightPixel(pixelHeight) > 13 || getHeightBlock_from_HeightPixel(pixelHeight) < 1) return false;
	return true;
}

//11. bắt event ấn các phím điều hướng,...
void GSBattle::eatShoes() {
	if (getWidthBlock_from_WidthPixel(Player1->Get2DPosition().x) == getWidthBlock_from_WidthPixel(m_shoes->Get2DPosition().x)
		&& getHeightBlock_from_HeightPixel(Player1->Get2DPosition().y) == getHeightBlock_from_HeightPixel(m_shoes->Get2DPosition().y)) {
		delta_pixel_1 = 20;
		m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));
		ResourceManagers::GetInstance()->PlaySound("eat", false);
	}
	else if (getWidthBlock_from_WidthPixel(Player2->Get2DPosition().x) == getWidthBlock_from_WidthPixel(m_shoes->Get2DPosition().x)
		&& getHeightBlock_from_HeightPixel(Player2->Get2DPosition().y) == getHeightBlock_from_HeightPixel(m_shoes->Get2DPosition().y)) {
		m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));
		ResourceManagers::GetInstance()->PlaySound("eat", false);
	}
}

void GSBattle::HandleKeyEvents(int key, bool bIsPressed)	//ấn bàn phím
{
	double momentX1 = (Player1->Get2DPosition()).x;
	double momentY1 = (Player1->Get2DPosition()).y;

	//Ấn nút lên thì y giảm, x giữ nguyên nhưng làm tròn theo block
	if (key == KEY_UP && bIsPressed == true) {
		if (momentY1 >= 100
			&& isGrass(momentX1, momentY1 - 26)
			) {	//ấn nút trái thì có thể đi được
			Player1->Set2DPosition(
				getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(momentX1)),
				momentY1 - delta_pixel_1);
			eatShoes();
			if ((Player1->Get2DPosition()).y < 100) {	//ra ngoài màn hình
				Player1->Set2DPosition(
					momentX1,
					100);
			}
		}

	}


	//ấn xuống thì y tăng, x giữ nguyên nhưng làm tròn theo block
	else if (key == KEY_DOWN && bIsPressed == true) {
		if (momentY1 <= 700
			&& isGrass(momentX1, momentY1 + 25)
			) {
			Player1->Set2DPosition(
				getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(momentX1)),
				momentY1 + delta_pixel_1);
			eatShoes();
			if ((Player1->Get2DPosition()).y > 700) {
				Player1->Set2DPosition(
					momentX1,
					700);
			}
		}

	}


	//ấn sang trái thì x giảm, y giữ nguyên nhưng làm tròn theo block
	else if (key == KEY_LEFT && bIsPressed == true) {
		if (momentX1 >= 110
			&& isGrass(momentX1 - 26, momentY1)
			) {
			Player1->Set2DPosition(
				momentX1 - delta_pixel_1,
				getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(momentY1)));
			eatShoes();
			if ((Player1->Get2DPosition()).x < 110) {
				Player1->Set2DPosition(
					110,
					momentY1);
			}
		}
	}


	//ấn sang phải thì x tăng, y giữ nguyên nhưng làm tròn theo block
	else if (key == KEY_RIGHT && bIsPressed == true) {
		if (momentX1 <= 1410
			&& isGrass(momentX1 + 25, momentY1)
			) {
			Player1->Set2DPosition(
				momentX1 + delta_pixel_1,
				getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(momentY1)));
			eatShoes();
			if ((Player1->Get2DPosition()).x > 1410) {
				Player1->Set2DPosition(
					1410,
					momentY1);
			}
		}

	}

	else if (key == VK_SPACE && bIsPressed == true && bombstatus.size() < 2) {
		if ((bombstatus.size() == 1 && bombstatus.front().index == 1) || bombstatus.size() == 0) {
			cout << "Dat bomb bang phim shift, bombstatus.size() = " << bombstatus.size() << "\n";
			double yourPixelWidth = (Player1->Get2DPosition()).x;
			double yourPixelHeight = (Player1->Get2DPosition()).y;
			SetBomb2(yourPixelWidth, yourPixelHeight);
		}
	}
	
}

//12. bắt mouse event
void GSBattle::HandleTouchEvents(int x, int y, bool bIsPressed)	//ấn chuột
{
	for (auto it : m_listButton)
	{
		(it)->HandleTouchEvents(x, y, bIsPressed);
		if ((it)->IsHandle()) break;
	}

	if (bIsPressed == true && x >= screenWidth - 150 && x <= screenWidth && y >= 5 && y <= 55) {
		ResourceManagers::GetInstance()->PauseSound("Level1/BattleMusic");	//ấn nút back thì dừng nhạc
	}


	double momentX2 = (Player2->Get2DPosition()).x;
	double momentY2 = (Player2->Get2DPosition()).y;
	double WidthBlockClick = getWidthBlock_from_WidthPixel(x);
	double HeightBlockClick = getHeightBlock_from_HeightPixel(y);
	if (WidthBlockClick == 26 && HeightBlockClick == 11 && bIsPressed == true) {
		if (momentY2 >= 100
			&& isGrass(momentX2, momentY2 - 26)
			) {	//ấn nút trái thì có thể đi được
			Player2->Set2DPosition(
				getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(momentX2)),
				momentY2 - delta_pixel_2);
			eatShoes();
			if ((Player2->Get2DPosition()).y < 100) {	//ra ngoài màn hình
				Player2->Set2DPosition(
					momentX2,
					100);
			}
		}

	}


	//ấn xuống thì y tăng, x giữ nguyên nhưng làm tròn theo block
	else if (WidthBlockClick == 26 && HeightBlockClick == 13 && bIsPressed == true) {
		if (momentY2 <= 700
			&& isGrass(momentX2, momentY2 + 25)
			) {
			Player2->Set2DPosition(
				getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(momentX2)),
				momentY2 + delta_pixel_2);
			eatShoes();
			if ((Player2->Get2DPosition()).y > 700) {
				Player2->Set2DPosition(
					momentX2,
					700);
			}
		}

	}


	//ấn sang trái thì x giảm, y giữ nguyên nhưng làm tròn theo block
	else if (WidthBlockClick == 25 && HeightBlockClick == 12 && bIsPressed == true) {
		if (momentX2 >= 110
			&& isGrass(momentX2 - 26, momentY2)
			) {
			Player2->Set2DPosition(
				momentX2 - delta_pixel_2,
				getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(momentY2)));
			eatShoes();
			if ((Player2->Get2DPosition()).x < 110) {
				Player2->Set2DPosition(
					110,
					momentY2);
			}
		}
	}


	//ấn sang phải thì x tăng, y giữ nguyên nhưng làm tròn theo block
	else if (WidthBlockClick == 27 && HeightBlockClick == 12 && bIsPressed == true) {
		if (momentX2 <= 1410
			&& isGrass(momentX2 + 25, momentY2)
			) {
			Player2->Set2DPosition(
				momentX2 + delta_pixel_2,
				getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(momentY2)));
			eatShoes();
			if ((Player2->Get2DPosition()).x > 1410) {
				Player2->Set2DPosition(
					1410,
					momentY2);
			}
		}
	}
	else if (WidthBlockClick == 26 && HeightBlockClick == 12 && bIsPressed == true && bombstatus.size() < 2) {	//ấn nút cách để đặt bom
		if ((bombstatus.size() == 1 && bombstatus.front().index == 2) || bombstatus.size() == 0) {
			cout << "Dat bomb bang phim space, bombstatus.size() = " << bombstatus.size() << "\n";
			double yourPixelWidth = (Player2->Get2DPosition()).x;
			double yourPixelHeight = (Player2->Get2DPosition()).y;
			SetBomb(yourPixelWidth, yourPixelHeight);
		}
	}
}

void GSBattle::Update(float deltaTime)	//hoạt ảnh chuyển động
{
	FinishExplode(deltaTime);
	FinishBomb(deltaTime);
	FinishBomb2(deltaTime);
	FinishExplode2(deltaTime);
	for (auto it : m_listSpriteAnimations)
	{
		it->Update(deltaTime);
	}
	for (auto it : m_bombs)
	{
		it->Update(deltaTime);
	}
	for (auto it : m_fires)
	{
		it->Update(deltaTime);
	}
	for (auto it : m_fires2)
	{
		it->Update(deltaTime);
	}
	for (auto it : m_bombs2)
	{
		it->Update(deltaTime);
	}
	ShowResult(deltaTime);
	Player1->Update(deltaTime);
	Player2->Update(deltaTime);

	HasBombermanBeenFired(deltaTime);
}

void GSBattle::Draw()	//render lên màn hình
{
	m_BackGround->Draw();
	m_coins->Draw();
	m_skull->Draw();
	m_score->Draw();
	m_shoes->Draw();
	btnLeft->Draw();
	btnRight->Draw();
	btnUp->Draw();
	btnDown->Draw();
	btnSetBomb->Draw();
	for (auto it : m_listButton)
	{
		it->Draw();
	}
	for (auto it : m_fires)
	{
		it->Draw();
	}
	for (auto it : m_fires2)
	{
		it->Draw();
	}

	Player1->Draw();
	Player2->Draw();
	for (auto it : m_listSpriteAnimations)
	{
		it->Draw();
	}
	for (auto it : m_rocks) {
		it->Draw();
	}
	for (auto it : m_borders) {
		it->Draw();
	}
	for (auto it : m_bricks) {
		it->Draw();
	}
	for (auto it : m_bombs) {
		it->Draw();
	}
	for (auto it : m_bombs2) {
		it->Draw();
	}
}

void GSBattle::SetBomb2(int pixelWidth, int pixelHeight) {
	BattleBomb b; b.index = 2; b.xBlock = getWidthBlock_from_WidthPixel(pixelWidth); b.yBlock = getHeightBlock_from_HeightPixel(pixelHeight);
	bombstatus.push(b);

	if (hasBomb2BeenFinished == 1) {	//nếu bomb 2 đã nổ rồi hoặc chưa được đặt
		finishBomb2Time = 4;	//quay thời gian hẹn giờ về 4
		auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
		auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
		auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/bomb");
		std::shared_ptr<SpriteAnimation> bomb = std::make_shared<SpriteAnimation>(model, shader, texture, 2, 1.0f / 30);
		bomb->Set2DPosition(
			getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(pixelWidth)),
			getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(pixelHeight)));
		bomb->SetSize(75, 75);
		m_bombs2.push_back(bomb);
		hasBomb2BeenFinished = 0;	//đặt rồi thì xem như bom 2 chưa nổ và tính thời gian lại từ đầu
	}
};	//đặt bom
void GSBattle::SetExplode2(int pixelWidth, int pixelHeight) {	//bắn tia lửa
	if (hasFire2BeenFinished == 1) {	//nếu ko có tia lửa của quả bom số 2 thì bom 2 mới được phép nổ
		finishFire2Time = 1;
		finishBomb2Time = 4;
		//nổ về 4 phía
		ExplodeLeft(pixelWidth, pixelHeight, 2);
		ExplodeRight(pixelWidth, pixelHeight, 2);
		ExplodeUp(pixelWidth, pixelHeight, 2);
		ExplodeDown(pixelWidth, pixelHeight, 2);
		//bom 2 nổ, xem xét vị trí của nó so với bom 1 và trạng thái của bombstatus
		if (bombstatus.size() == 2) {	//nếu đang có 2 quả bom trên màn hình
			if (bombstatus.front().index == 2) {	//giả sử bom 2 nổ trước, tác động đến bom 1



				if (bombstatus.front().xBlock == bombstatus.back().xBlock) {	//cùng 1 cột
					if (abs(bombstatus.front().yBlock - bombstatus.back().yBlock) <= 8) {
						//cùng 1 hàng và cách nhau chưa tới 8 theo chiều ngang
						finishBomb1Time = 0.0000001;
					}
				}
				if (bombstatus.front().yBlock == bombstatus.back().yBlock) {	//cùng 1 hàng
					if (abs(bombstatus.front().xBlock - bombstatus.back().xBlock) <= 8) {
						//cùng 1 cột và cách nhau chưa tới 8 theo chiều dọc
						finishBomb1Time = 0.0000001;
					}
				}



			}
		}
		//
	}
	hasFire2BeenFinished = 0;	//tia lửa của bom 2 bắt đầu xuất hiện
								//quả bom thứ 2 đã nổ nghĩa là trong túi tăng từ 0 lên 1 quả bom
								//MAXBattleBomb++;	//1
	bombstatus.pop();
};	//nổ bom+
void GSBattle::FinishBomb2(float deltaTime) {	//kick nổ bom 2

	if (finishBomb2Time > 0)
		finishBomb2Time -= 1 * deltaTime;
	if (finishBomb2Time < 0 && hasBomb2BeenFinished == 0) {	//nếu hết thời gian mà bom chưa nổ thì kick nổ quả bom
		ResourceManagers::GetInstance()->PlaySound("exploded", false);
		SetExplode2(m_bombs2[0]->Get2DPosition().x, m_bombs2[0]->Get2DPosition().y);
		m_bombs2.erase(m_bombs2.begin() + 0);
		hasBomb2BeenFinished = 1;
	}
};	//nổ bom-
void GSBattle::FinishExplode2(float deltaTime) {
	//bom 2
	if (finishFire2Time > 0)	//nếu tia lửa vẫn còn xuất hiện
		finishFire2Time -= 1 * deltaTime;
	if (finishFire2Time < 0 && hasFire2BeenFinished == 0) {	//nếu tia lửa đã cháy hết thì ẩn đi
		finishFire2Time = 0;
		m_fires2.clear();
		hasFire2BeenFinished = 1;
	}
};	//bom cháy hết

	//15. kiểm tra xem người chơi có chạm vào tia lửa hay ko
void GSBattle::HasBombermanBeenFired(float deltaTime) {	//kiểm tra xem người chơi có chạm vào tia lửa hay ko
	if (!m_fires.empty()) {
		int myPositionBlockWidth = getWidthBlock_from_WidthPixel((Player1->Get2DPosition().x));
		int myPositionBlockHeight = getHeightBlock_from_HeightPixel((Player1->Get2DPosition().y));
		int myPositionBlockWidth2 = getWidthBlock_from_WidthPixel((Player2->Get2DPosition().x));
		int myPositionBlockHeight2 = getHeightBlock_from_HeightPixel((Player2->Get2DPosition().y));
		for (int i = 0; i < m_fires.size(); i++) {
			int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_fires[i]->Get2DPosition().x));
			int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_fires[i]->Get2DPosition().y));
			if (myPositionBlockWidth == enemyPositionBlockWidth && myPositionBlockHeight == enemyPositionBlockHeight) {
				DiedBomberman(deltaTime);
				break;
			} else if (myPositionBlockWidth2 == enemyPositionBlockWidth && myPositionBlockHeight2 == enemyPositionBlockHeight) {
				DiedBomberman2(deltaTime);
				break;
			}
		}
	}

	if (!m_fires2.empty()) {
		int myPositionBlockWidth = getWidthBlock_from_WidthPixel((Player1->Get2DPosition().x));
		int myPositionBlockHeight = getHeightBlock_from_HeightPixel((Player1->Get2DPosition().y));
		int myPositionBlockWidth2 = getWidthBlock_from_WidthPixel((Player2->Get2DPosition().x));
		int myPositionBlockHeight2 = getHeightBlock_from_HeightPixel((Player2->Get2DPosition().y));
		for (int i = 0; i < m_fires2.size(); i++) {
			int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_fires2[i]->Get2DPosition().x));
			int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_fires2[i]->Get2DPosition().y));
			if (myPositionBlockWidth == enemyPositionBlockWidth && myPositionBlockHeight == enemyPositionBlockHeight) {
				DiedBomberman(deltaTime);
				break;
			}
			else if (myPositionBlockWidth2 == enemyPositionBlockWidth && myPositionBlockHeight2 == enemyPositionBlockHeight) {
				DiedBomberman2(deltaTime);
				break;
			}
		}
	}
}

//18. Cho Bomberman biến mất khỏi màn hình khi bị bom nổ hoặc chạm vào tia lửa
void GSBattle::DiedBomberman(float deltaTime) {
	ResourceManagers::GetInstance()->PauseSound("Level1/BattleMusic");
	ResourceManagers::GetInstance()->PlaySound("death", false);
	m_skull->Set2DPosition(Player1->Get2DPosition().x, Player1->Get2DPosition().y);
	Player1->Set2DPosition(getWidthPixel_from_WidthBlock((300)), getHeightPixel_from_HeightBlock((300)));
	m_time += deltaTime;
	deaths++;
	ofstream Level("C:\\Users\\dell\\Desktop\\Programming_anim\\TrainingFramework\\src\\GameStates\\battle_result.txt");
	Level << to_string(deaths == 1 ? 2 : 0);
	Level.close();
}
void GSBattle::DiedBomberman2(float deltaTime) {
	ResourceManagers::GetInstance()->PauseSound("Level1/BattleMusic");
	ResourceManagers::GetInstance()->PlaySound("death", false);
	m_skull->Set2DPosition(Player2->Get2DPosition().x, Player2->Get2DPosition().y);
	Player2->Set2DPosition(getWidthPixel_from_WidthBlock((300)), getHeightPixel_from_HeightBlock((300)));
	m_time += deltaTime;
	deaths++;
	ofstream Level("C:\\Users\\dell\\Desktop\\Programming_anim\\TrainingFramework\\src\\GameStates\\battle_result.txt");
	Level << to_string(deaths == 1 ? 1 : 0);
	Level.close();
}

//19. 1 trong 2 hoặc cả 2 người chơi chết thì đi đến màn hình kết quả
void GSBattle::ShowResult(float deltaTime) {
	if (m_time > 0)	//Khi hàm DiedBomberman đc gọi 1 lần thì hàm này sẽ được kích hoạt
	{
		m_time += deltaTime;
		if (m_time > 4.5) {
			GameStateMachine::GetInstance()->ChangeState(StateTypes::STATE_BattleResult);
			m_time = 0;
		}
	}
}

//20. Đặt bom vào ô gạch chứa giày
void GSBattle::CheckBreakShoes() {
	if (HasShoesBeenExplored == 0) {	//nếu giày chưa được tìm ra
		m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(shoesWidthByBlock), getHeightPixel_from_HeightBlock(shoesHeightByBlock));
		HasShoesBeenExplored = 1;
	}
}