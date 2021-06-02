#include "GSPlay.h"
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
#define FIRE_LENGTH 6
using namespace std;
extern int screenWidth; //need get on Graphic engine
extern int screenHeight; //need get on Graphic engine

int real_screenWidth = screenWidth * 1350/1520; // = 1350 pixel
int real_screenHeight = screenHeight * 650/800; // = 650 pixel

GSPlay::GSPlay()
{
}


GSPlay::~GSPlay()
{

}

//1. Chuyển đổi pixel sang các ô và ngược lại

//Chiều ngang: 1350 pixel, 50 pixel/block, 27 block = 14 gạch/cỏ + 13 đá
int GSPlay::getWidthBlock_from_WidthPixel(int now_WidthPixel) {
	if (0.5 * (screenWidth - real_screenWidth) <= now_WidthPixel &&
		now_WidthPixel < real_screenWidth + 0.5 * (screenWidth - real_screenWidth))//từ 85 đến 1435
		return (int)((now_WidthPixel - 0.5 * (screenWidth - real_screenWidth))	//thụt 85
			*27.0 / 1350 + 1.0);
	else if (now_WidthPixel == real_screenWidth + 0.5 * (screenWidth - real_screenWidth))
		return 27;
	else return 0;
}

//chiều cao: 650 pixel, 50 pixel/block, 13 block = 7 gạch/ cỏ + 6 đá
int GSPlay::getHeightBlock_from_HeightPixel(int now_HeightPixel){
	if (0.5 * (screenHeight - real_screenHeight) <= now_HeightPixel &&
		now_HeightPixel < real_screenHeight + 0.5 * (screenHeight - real_screenHeight)) //từ 75 đến 725
		return (int)((now_HeightPixel - 0.5 * (screenHeight - real_screenHeight)) //thụt 75
			*13.0 / 650 + 1.0);
	else if (now_HeightPixel == real_screenHeight + 0.5 * (screenHeight - real_screenHeight))
		return 13;
	else return 0;
}

int GSPlay::getWidthPixel_from_WidthBlock(int now_WidthBlock) {	//chiều ngang
	return 50 * (now_WidthBlock*1.0 - 0.5f)*1.0f
		+ 0.5 * (screenWidth - real_screenWidth);
}

int GSPlay::getHeightPixel_from_HeightBlock(int now_HeightBlock) {//chiều cao
	return 50 * (now_HeightBlock*1.0 - 0.5f)*1.0f
		+ 0.5 * (screenHeight - real_screenHeight);
}

//2. đặt các viên gạch lên màn hình
void GSPlay::InitBricks() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/brick");
	for (int i = 1; i <= 27; i += 1) {	//width
		for (int j = 2; j <= 12; j += 1) {	//height
			if ((i % 2 != 0 || j % 2 != 0) && rand() % 3 == 1) {
				int width_pixel = getWidthPixel_from_WidthBlock(i);	//lấy tâm pixel của block
				int height_pixel = getHeightPixel_from_HeightBlock(j);	//lấy tâm pixel của block
				std::shared_ptr<Sprite2D> brick = std::make_shared<Sprite2D>(model, shader, texture);
				brick->Set2DPosition(width_pixel, height_pixel);
				brick->SetSize(50, 50);
				m_bricks.push_back(brick);
			}
		}
	}
	int doorPosition = rand() % m_bricks.size();	//vị trí của cửa là 1 số ngẫu nhiên từ 0 đến tổng số viên gạch - 1
	doorWidthByBlock = getWidthBlock_from_WidthPixel(m_bricks[doorPosition]->Get2DPosition().x);	//lưu tọa độ theo block của cửa
	doorHeightByBlock = getHeightBlock_from_HeightPixel(m_bricks[doorPosition]->Get2DPosition().y);
	
	int clockPosition = rand() % m_bricks.size();
	while (doorPosition == clockPosition) {	//chừng nào vị trí của cửa trùng với vị trí của đồng hồ thì cứ lặp lại
		clockPosition = rand() % m_bricks.size();
	}
	//int clockPosition = 0;
	clockWidthByBlock = getWidthBlock_from_WidthPixel(m_bricks[clockPosition]->Get2DPosition().x);	//lưu tọa độ theo block của đồng hồ
	clockHeightByBlock = getHeightBlock_from_HeightPixel(m_bricks[clockPosition]->Get2DPosition().y);
	
	int shoesPosition = rand() % m_bricks.size();
	while (shoesPosition == clockPosition) {	//chừng nào vị trí của cửa trùng với vị trí của đồng hồ thì cứ lặp lại
		shoesPosition = rand() % m_bricks.size();
	}
	//int shoesPosition = 1;

	shoesWidthByBlock = getWidthBlock_from_WidthPixel(m_bricks[shoesPosition]->Get2DPosition().x);	//lưu tọa độ theo block của giày
	shoesHeightByBlock = getHeightBlock_from_HeightPixel(m_bricks[shoesPosition]->Get2DPosition().y);
}

//3. đặt các viên đá lên màn hình
void GSPlay::InitRocks() {
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


//4. đặt các enemies lên màn hình
void GSPlay::InitEnemies() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("Animation");
	

	for (int i = 0; i < 6*level; i++) {
		auto texture = ResourceManagers::GetInstance()->GetTexture("enemies/enemy" + to_string((i%10)));
		std::shared_ptr<SpriteAnimation> enemy = std::make_shared<SpriteAnimation>(model, shader, texture, 4, 1.0f / 6);
		enemy->Set2DPosition(getWidthPixel_from_WidthBlock(27 - i), getHeightPixel_from_HeightBlock(13));
		enemy->SetSize(50, 50);
		m_enemies.push_back(enemy);
	}
	for (int i = 0; i < m_enemies.size(); i++) {
		m_enemies_direction.push_back(1);	//khởi tạo hướng đi ngẫu nhiên cho enemy
		Venemies.push_back(INITIAL_SPEED);	//vận tốc ban đầu của tất cả enemies là 6 pixel/deltaTime
	}
}

void GSPlay::InitDoor(){
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/door");
	m_door = std::make_shared<Sprite2D>(model, shader, texture);
	m_door->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));	//Cửa khởi tạo ở vô cực
	m_door->SetSize(90, 55);
}

void GSPlay::InitClock() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/clock");
	m_clock = std::make_shared<Sprite2D>(model, shader, texture);
	m_clock->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));	//Đồng hồ khởi tạo ở vô cực
	m_clock->SetSize(140, 140);
}

void GSPlay::InitShoes() {
	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/shoes");
	m_shoes = std::make_shared<Sprite2D>(model, shader, texture);
	m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));	//Giày khởi tạo ở vô cực
	m_shoes->SetSize(50, 100);
}

int GSPlay::GetLevel() {
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
		level = n;
	}

	fileInput.close();
}

//5. tương đương với constructor khởi tạo trong java
void GSPlay::Init()	
{
	GetLevel();
	InitBricks();
	InitRocks();
	InitEnemies();
	InitDoor();
	InitClock();
	InitShoes();

	auto model = ResourceManagers::GetInstance()->GetModel("Sprite2D");
	auto texture = ResourceManagers::GetInstance()->GetTexture("bomberman/background"+to_string(level));
	auto shader = ResourceManagers::GetInstance()->GetShader("TextureShader");
	m_BackGround = std::make_shared<Sprite2D>(model, shader, texture);
	m_BackGround->Set2DPosition(screenWidth / 2, screenHeight / 2);
	m_BackGround->SetSize(real_screenWidth, real_screenHeight);

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
	button->Set2DPosition(screenWidth-75, 30);
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
	bomberman = std::make_shared<SpriteAnimation>(model, shader, texture, 12, 1.0f/30);
	bomberman->Set2DPosition(
		getWidthPixel_from_WidthBlock(1),
		getHeightPixel_from_HeightBlock(1)
	);
	bomberman->SetSize(50, 50);
	//bomberman

	shader = ResourceManagers::GetInstance()->GetShader("TextShader");
	std::shared_ptr<Font> font = ResourceManagers::GetInstance()->GetFont("arialbd");
	//score
	m_score = std::make_shared< Text>(shader, font, "Score: 0", TEXT_COLOR::RED, 1.0);	//so diem
	m_score->Set2DPosition(getWidthPixel_from_WidthBlock(0)-25, getHeightPixel_from_HeightBlock(0));

	//level
	m_level = std::make_shared< Text>(shader, font, "Level: "+ to_string(level), TEXT_COLOR::CYAN, 1.0);	//level
	m_level->Set2DPosition(getWidthPixel_from_WidthBlock(2), getHeightPixel_from_HeightBlock(0));
	//coins total quantity
	m_coins = std::make_shared< Text>(shader, font, "x : 0", TEXT_COLOR::PURPLE, 1.0);	//so xu da an duoc
	m_coins->Set2DPosition(getWidthPixel_from_WidthBlock(6), getHeightPixel_from_HeightBlock(0));

	// Animation đồng xu
	shader = ResourceManagers::GetInstance()->GetShader("Animation");
	texture = ResourceManagers::GetInstance()->GetTexture("bomberman/bomberman_dead");
	std::shared_ptr<SpriteAnimation> objCoin = std::make_shared<SpriteAnimation>(model, shader, texture, 8, 1.0f/4);
	objCoin->Set2DPosition(getWidthPixel_from_WidthBlock(5) + 20, getHeightPixel_from_HeightBlock(0) - 8);
	objCoin->SetSize(22, 22);
	m_listSpriteAnimations.push_back(objCoin);
	
	ResourceManagers::GetInstance()->PlaySound("Level1/LevelBGM"+to_string(level), true);	//true là có lặp lại
}

void GSPlay::Exit()
{

}


void GSPlay::Pause()
{

}

void GSPlay::Resume()
{

}


void GSPlay::HandleEvents()
{

}

//6. hàm check bomb đã nổ chưa
void GSPlay::FinishBomb(float deltaTime) {
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
void GSPlay::FinishExplode(float deltaTime) {
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
void GSPlay::SetBomb(int pixelWidth, int pixelHeight) {	//đặt bomb bằng phím space
		BOMB b; b.index = 1; b.xBlock = getWidthBlock_from_WidthPixel(pixelWidth); b.yBlock = getHeightBlock_from_HeightPixel(pixelHeight);
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
void GSPlay::SetExplode(int pixelWidth, int pixelHeight) {	//khi bom 1 nổ thì gọi hàm này
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
						if (abs(bombstatus.front().yBlock - bombstatus.back().yBlock) <= FIRE_LENGTH) {
							//cùng 1 hàng và cách nhau chưa tới FIRE_LENGTH theo chiều ngang
							finishBomb2Time = 0.0000001;
						}
					}
					if (bombstatus.front().yBlock == bombstatus.back().yBlock) {	//cùng 1 hàng
						if (abs(bombstatus.front().xBlock - bombstatus.back().xBlock) <= FIRE_LENGTH) {
							//cùng 1 cột và cách nhau chưa tới FIRE_LENGTH theo chiều dọc
							finishBomb2Time = 0.0000001;
						}
					}
				}
			}
			//
	}
	hasFireBeenFinished = 0;
	//MAXBOMB++;
	bombstatus.pop();
}


void GSPlay::ExplodeLeft(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 0; i <= FIRE_LENGTH; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth - 50 * i, pixelHeight) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth - 50 * i) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == doorWidthByBlock && blockHeights[j] == doorHeightByBlock)
						CheckBreakDoor();
					if (blockWidths[j] == clockWidthByBlock && blockHeights[j] == clockHeightByBlock)
						CheckBreakClock();
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
			if (getWidthBlock_from_WidthPixel(ROUNDED_widthPixel) == doorWidthByBlock
				&& getHeightBlock_from_HeightPixel(ROUNDED_heightPixel) == doorHeightByBlock
				&& HasDoorBeenExplored == 1)
				BreakDoor(getWidthBlock_from_WidthPixel(pixelWidth - 50 * i), getHeightBlock_from_HeightPixel(pixelHeight));
		}
		else fireL->Set2DPosition(0, 0);


		fireL->SetSize(50, 50);
		if (whichFire == 1)
			m_fires.push_back(fireL);
		else if (whichFire == 2)
			m_fires2.push_back(fireL);
	}
};
void GSPlay::ExplodeRight(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
			blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
			blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
			index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 0; i <= FIRE_LENGTH; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth + 50 * i, pixelHeight) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
			//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth + 50 * i) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == doorWidthByBlock && blockHeights[j] == doorHeightByBlock)
						CheckBreakDoor();
					if (blockWidths[j] == clockWidthByBlock && blockHeights[j] == clockHeightByBlock)
						CheckBreakClock();
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
			if (getWidthBlock_from_WidthPixel(ROUNDED_widthPixel) == doorWidthByBlock
				&& getHeightBlock_from_HeightPixel(ROUNDED_heightPixel) == doorHeightByBlock
				&& HasDoorBeenExplored == 1)
				BreakDoor(getWidthBlock_from_WidthPixel(pixelWidth + 50 * i), getHeightBlock_from_HeightPixel(pixelHeight));
		}
		else fireR->Set2DPosition(0, 0);
		fireR->SetSize(50, 50);



		if (whichFire == 1)
			m_fires.push_back(fireR);
		else if (whichFire == 2)
			m_fires2.push_back(fireR);
	}
};
void GSPlay::ExplodeUp(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 0; i <= FIRE_LENGTH; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth, pixelHeight - 50 * i) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight - 50 * i) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == doorWidthByBlock && blockHeights[j] == doorHeightByBlock)
						CheckBreakDoor();
					if (blockWidths[j] == clockWidthByBlock && blockHeights[j] == clockHeightByBlock)
						CheckBreakClock();
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
			if (getWidthBlock_from_WidthPixel(ROUNDED_widthPixel) == doorWidthByBlock
				&& getHeightBlock_from_HeightPixel(ROUNDED_heightPixel) == doorHeightByBlock
				&& HasDoorBeenExplored == 1)
				BreakDoor(getWidthBlock_from_WidthPixel(pixelWidth), getHeightBlock_from_HeightPixel(pixelHeight - 50 * i));
		}
		else fireU->Set2DPosition(0, 0);


		fireU->SetSize(50, 50);
		if (whichFire == 1)
			m_fires.push_back(fireU);
		else if (whichFire == 2)
			m_fires2.push_back(fireU);
	}
};
void GSPlay::ExplodeDown(int pixelWidth, int pixelHeight, int whichFire) {
	vector<int> blockWidths;	//chứa blockWidth của các viên gạch
	vector<int> blockHeights;	//chứa blockHeight của các viên gạch
	vector<int> index_m_bricks;
	for (int i = 0; i < m_bricks.size(); i++) {	//duyệt từng viên gạch một
		blockWidths.push_back(getWidthBlock_from_WidthPixel(m_bricks[i]->Get2DPosition().x));
		blockHeights.push_back(getHeightBlock_from_HeightPixel(m_bricks[i]->Get2DPosition().y));
		index_m_bricks.push_back(i);
	}
	//tất cả tọa độ theo block của các viên gạch đã được lưu trong blockWidths và blockHeights
	for (int i = 0; i <= FIRE_LENGTH; i++) {
		//ko gặp cỏ mà gặp gạch hoặc đá
		if (isGrass(pixelWidth, pixelHeight + 50 * i) == false) {	//dù gặp gạch hay gặp đá thì vòng lặp cũng sẽ dừng lại ngay lập tức
																	//gặp đá thì break luôn (tọa độ block theo cả 2 chiều đều là số chẵn)
			for (int j = 0; j < blockWidths.size(); j++) {
				if (getWidthBlock_from_WidthPixel(pixelWidth) == blockWidths[j] && getHeightBlock_from_HeightPixel(pixelHeight + 50 * i) == blockHeights[j]) {
					//nếu chạm vào gạch thì phá viên gạch đó
					m_bricks.erase(m_bricks.begin() + index_m_bricks[j]);
					//nếu phá vào ô gạch chứa cửa thì cửa xuất hiện
					if (blockWidths[j] == doorWidthByBlock && blockHeights[j] == doorHeightByBlock)
						CheckBreakDoor();
					if (blockWidths[j] == clockWidthByBlock && blockHeights[j] == clockHeightByBlock)
						CheckBreakClock();
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
			if(getWidthBlock_from_WidthPixel(ROUNDED_widthPixel) == doorWidthByBlock
				&& getHeightBlock_from_HeightPixel(ROUNDED_heightPixel) == doorHeightByBlock
				&& HasDoorBeenExplored == 1)
				BreakDoor(getWidthBlock_from_WidthPixel(pixelWidth), getHeightBlock_from_HeightPixel(pixelHeight + 50 * i));
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
bool GSPlay::isGrass(int pixelWidth, int pixelHeight) {
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
void GSPlay::eatClock() {
	for (int i = 0; i < Venemies.size(); i++) {
		Venemies[i] = Venemies[i] / 1.5;
	}
	m_clock->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));
	ResourceManagers::GetInstance()->PlaySound("eat", false);
}
void GSPlay::eatShoes() {
	delta_pixel = 20;
	m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(100), getHeightPixel_from_HeightBlock(100));
	ResourceManagers::GetInstance()->PlaySound("eat", false);
}

void GSPlay::eatClockOrShoes(){
	if (getWidthBlock_from_WidthPixel(bomberman->Get2DPosition().x) == getWidthBlock_from_WidthPixel(m_clock->Get2DPosition().x)
		&& getHeightBlock_from_HeightPixel(bomberman->Get2DPosition().y) == getHeightBlock_from_HeightPixel(m_clock->Get2DPosition().y)) {
		eatClock();
	}
	else if (getWidthBlock_from_WidthPixel(bomberman->Get2DPosition().x) == getWidthBlock_from_WidthPixel(m_shoes->Get2DPosition().x)
		&& getHeightBlock_from_HeightPixel(bomberman->Get2DPosition().y) == getHeightBlock_from_HeightPixel(m_shoes->Get2DPosition().y)) {
		eatShoes();
	}
}

void GSPlay::HandleKeyEvents(int key, bool bIsPressed)	//ấn bàn phím
{
	double momentX = (bomberman->Get2DPosition()).x;
	double momentY = (bomberman->Get2DPosition()).y;
	int blockWidth = getWidthBlock_from_WidthPixel(momentX);
	int blockHeight = getHeightBlock_from_HeightPixel(momentY);
	//Ấn nút lên thì y giảm, x giữ nguyên nhưng làm tròn theo block
	if (key == KEY_UP && bIsPressed == true) {
		if (momentY >= 100
			&& isGrass(momentX, momentY - 26)
			) {	//ấn nút trái thì có thể đi được
			bomberman->Set2DPosition(
				getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(momentX)),
				momentY - delta_pixel);
			eatClockOrShoes();
			if ((bomberman->Get2DPosition()).y < 100) {	//ra ngoài màn hình
				bomberman->Set2DPosition(
					momentX,
					100);
			}
		}

	}


	//ấn xuống thì y tăng, x giữ nguyên nhưng làm tròn theo block
	else if (key == KEY_DOWN && bIsPressed == true) {
		if (momentY <= 700
			&& isGrass(momentX, momentY + 25)
			) {
			bomberman->Set2DPosition(
				getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(momentX)),
				momentY + delta_pixel);
			eatClockOrShoes();
			if ((bomberman->Get2DPosition()).y > 700) {
				bomberman->Set2DPosition(
					momentX,
					700);
			}
		}

	}


	//ấn sang trái thì x giảm, y giữ nguyên nhưng làm tròn theo block
	else if (key == KEY_LEFT && bIsPressed == true) {
		if (momentX >= 110 
			&& isGrass(momentX - 26, momentY)
			) {
			bomberman->Set2DPosition(
				momentX - delta_pixel,
				getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(momentY)));
			eatClockOrShoes();
			if ((bomberman->Get2DPosition()).x < 110) {
				bomberman->Set2DPosition(
					110,
					momentY);
			}
		}
	}


	//ấn sang phải thì x tăng, y giữ nguyên nhưng làm tròn theo block
	else if (key == KEY_RIGHT && bIsPressed == true) {
		if (momentX <= 1410 
			&& isGrass(momentX + 25, momentY)
			) {
			bomberman->Set2DPosition(
				momentX + delta_pixel,
				getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(momentY)));
			eatClockOrShoes();
			if ((bomberman->Get2DPosition()).x > 1410) {
				bomberman->Set2DPosition(
					1410,
					momentY);
			}
		}

	}



	else if (key == VK_SPACE && bIsPressed == true && bombstatus.size() < 2) {	//ấn nút cách để đặt bom
		if ((bombstatus.size() == 1 && bombstatus.front().index == 2) || bombstatus.size() == 0){
			cout << "Dat bomb bang phim space, bombstatus.size() = " << bombstatus.size() << "\n";
			double yourPixelWidth = (bomberman->Get2DPosition()).x;
			double yourPixelHeight = (bomberman->Get2DPosition()).y;
			SetBomb(yourPixelWidth, yourPixelHeight);
		}
	}

	else if ((key == VK_F1 || key == VK_F2 || key == VK_F3) && bIsPressed == true) {	//Chọn level1
		ResourceManagers::GetInstance()->PauseSound("Level1/LevelBGM" + to_string(level));
		ofstream Level("C:\\Users\\dell\\Desktop\\Programming_anim\\TrainingFramework\\src\\GameStates\\level.txt");
		if (key == VK_F1) Level << to_string(1);
		else if (key == VK_F2) Level << to_string(2);
		else if (key == VK_F3) Level << to_string(3);
		Level.close();
		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->ChangeState(StateTypes::STATE_StageGameStart);
	}

	else if (key == VK_SHIFT && bIsPressed == true && bombstatus.size() < 2) {
		if ((bombstatus.size() == 1 && bombstatus.front().index == 1) || bombstatus.size() == 0) {
			cout << "Dat bomb bang phim shift, bombstatus.size() = " << bombstatus.size() << "\n";
			double yourPixelWidth = (bomberman->Get2DPosition()).x;
			double yourPixelHeight = (bomberman->Get2DPosition()).y;
			SetBomb2(yourPixelWidth, yourPixelHeight);
		}
	}
	if (hasAllEnemiesBeKilled == 1 && bIsPressed == false) {
		NextLevel();
	}

}

//12. bắt mouse event
void GSPlay::HandleTouchEvents(int x, int y, bool bIsPressed)	//ấn chuột
{
	for (auto it : m_listButton)
	{
		(it)->HandleTouchEvents(x, y, bIsPressed);
		if ((it)->IsHandle()) break;
	}

	if (bIsPressed == true && x >= screenWidth - 150 && x <= screenWidth && y >= 5 && y <= 55) {
		ResourceManagers::GetInstance()->PauseSound("Level1/LevelBGM" + to_string(level));	//ấn nút back thì dừng nhạc
	}

	if (bIsPressed == true && hasAllEnemiesBeKilled == 1 //Diệt hết enemies và click vào cửa thì PlayAgain
		&& getWidthBlock_from_WidthPixel(x) == doorWidthByBlock
		&& getHeightBlock_from_HeightPixel(y) == doorHeightByBlock) {
		ResourceManagers::GetInstance()->PauseSound("Level1/LevelBGM" + to_string(level));
		ofstream Level("C:\\Users\\dell\\Desktop\\Programming_anim\\TrainingFramework\\src\\GameStates\\level.txt");
		Level << to_string(level >= 3 ? 1 : level + 1);
		Level.close();

		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->PopState();
		GameStateMachine::GetInstance()->ChangeState(StateTypes::STATE_StageGameStart);
	}
}

//13. set hướng chuyển động của enemies
void GSPlay::EnemiesChangeDirection(float deltaTime) {	//cứ đi vào ô ko phải cỏ hay gạch thì phải đổi hướng
	if (m_enemies.empty() == false) {
		for (int i = 0; i < m_enemies.size(); i++) {
			if (m_enemies_direction[i] == 1) {	//nếu enemy đang đi sang trái
				if (isGrass(	//mà gặp đá hoặc gạch
					getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[i]->Get2DPosition().x)) - 26,
					getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[i]->Get2DPosition().y))) == false)
				{//nếu bên trái enemies không phải là cỏ thì đổi hướng
					ForceChangeDirection(i, m_enemies_direction[i]);
				}
				//đổi hướng ngẫu nhiên theo thời gian29 <= deltaTime * 1000 && deltaTime * 1000 <= 47
				else RandomChangeDirection(i);
			}
			else if (m_enemies_direction[i] == 2) {////nếu enemy đang đi sang phải
				if (isGrass(//mà gặp đá hoặc gạch
					getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[i]->Get2DPosition().x)) + 26,
					getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[i]->Get2DPosition().y))) == false)
				{//nếu bên trái enemies không phải là cỏ thì đổi hướng
					ForceChangeDirection(i, m_enemies_direction[i]);
				}
				//đổi hướng ngẫu nhiên theo thời gian
				else RandomChangeDirection(i);

				//đổi hướng ngẫu nhiên theo thời gian
			}
			else if (m_enemies_direction[i] == 3) {//nếu enemy đang đi lên trên
				if (isGrass(// mà gặp đá hoặc gạch
					getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[i]->Get2DPosition().x)),
					getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[i]->Get2DPosition().y)) - 26) == false)
				{//nếu bên trái enemies không phải là cỏ thì đổi hướng
					ForceChangeDirection(i, m_enemies_direction[i]);
				}
				//đổi hướng theo thời gian
				else RandomChangeDirection(i);

				//đổi hướng theo thời gian
			}
			else if (m_enemies_direction[i] == 4) {//nếu enemy đang đi xuống dưới
				if (isGrass(// mà gặp đá hoặc gạch
					getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[i]->Get2DPosition().x)),
					getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[i]->Get2DPosition().y)) + 26) == false)
				{//nếu bên trái enemies không phải là cỏ thì đổi hướng
					ForceChangeDirection(i, m_enemies_direction[i]);
				}
				//đổi hướng theo thời gian
				else RandomChangeDirection(i);

				//đổi hướng theo thời gian
			}
		}
	}
}
//Đổi hướng ngẫu nhiên theo thời gian
void GSPlay::RandomChangeDirection(int enemy_index) {
	if (getWidthBlock_from_WidthPixel(m_enemies[enemy_index]->Get2DPosition().x) % 2 != 0
		&& getHeightBlock_from_HeightPixel(m_enemies[enemy_index]->Get2DPosition().y) %2 != 0) {	//nếu cả 4 hướng trái phải trên dưới ko phải đá thì mới đổi hướng
		int rd = rand() % OMEGA;	//xác suất đổi hướng là 1% thôi
		if (MIN_RD_DELTATIME <= rd && rd <= MAX_RD_DELTATIME) {
			m_enemies_direction[enemy_index] = rand() % 4 + 1;
		}
	}
}
//Enemy chạm vào gạch thì đổi hướng ngẫu nhiên 1 trong 3 hướng còn lại
void GSPlay::ForceChangeDirection(int enemy_index, int old_direction) {
	int numbers[] = {1, 2, 3};
	if (old_direction == 1) {	//đang sang trái mà gặp đá, gạch
		numbers[0] = 2;
		numbers[1] = 3;
		numbers[2] = 4;
	}
	else if (old_direction == 2) {//đang sang phải mà gặp đá, gạch
		numbers[0] = 1;
		numbers[1] = 3;
		numbers[2] = 4;
	}
	else if (old_direction == 3) {//đang lên trên mà gặp đá, gạch
		numbers[0] = 1;
		numbers[1] = 2;
		numbers[2] = 4;
	}
	else if (old_direction == 4) {//đang xuống dưới mà gặp đá, gạch
		numbers[0] = 1;
		numbers[1] = 2;
		numbers[2] = 3;
	}
	int length = sizeof(numbers) / sizeof(int);
	int randomNumber = numbers[rand() % length];

	m_enemies_direction[enemy_index] = randomNumber;	//đổi hướng
	m_enemies[enemy_index]->Set2DPosition(
		getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[enemy_index]->Get2DPosition().x)),
		getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[enemy_index]->Get2DPosition().y))
		//xuất phát tại vị trí cũ
	);
}

//14. set chuyển động theo hướng của enemies
void GSPlay::EnemiesMoving(float deltaTime) {
	if (m_enemies.empty() == false) {
		for (int i = 0; i < m_enemies.size(); i++) {
			if (m_enemies_direction[i] == 1) {	//trái
				m_enemies[i]->Set2DPosition(
					m_enemies[i]->Get2DPosition().x - Venemies[i]*deltaTime * 25,
					getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[i]->Get2DPosition().y))
				);
			}
			else if (m_enemies_direction[i] == 2) {//phải
				m_enemies[i]->Set2DPosition(
					m_enemies[i]->Get2DPosition().x + Venemies[i] *deltaTime * 25,
					getHeightPixel_from_HeightBlock(getHeightBlock_from_HeightPixel(m_enemies[i]->Get2DPosition().y))
				);
			}
			else if (m_enemies_direction[i] == 3) {//lên
				m_enemies[i]->Set2DPosition(
					getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[i]->Get2DPosition().x)),
					m_enemies[i]->Get2DPosition().y - Venemies[i] *deltaTime * 25
				);
			}
			else if (m_enemies_direction[i] == 4) {//xuống
				m_enemies[i]->Set2DPosition(
					getWidthPixel_from_WidthBlock(getWidthBlock_from_WidthPixel(m_enemies[i]->Get2DPosition().x)),
					m_enemies[i]->Get2DPosition().y + Venemies[i] *deltaTime * 25
				);
			}
		}
	}
}

void GSPlay::Update(float deltaTime)	//hoạt ảnh chuyển động
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
	for (auto it : m_enemies) {
		it->Update(deltaTime);
	}
	PlayAgain(deltaTime);
	bomberman->Update(deltaTime);
	EnemiesMoving(deltaTime);
	EnemiesChangeDirection(deltaTime);
	HasBombermanBeenFired(deltaTime);
	HasEnemiesBeenFired(deltaTime);
	HasBombermanBeenTouchedByEnemies(deltaTime);
}

void GSPlay::Draw()	//render lên màn hình
{
	m_BackGround->Draw();
	m_coins->Draw();
	m_skull->Draw();
	m_door->Draw();
	m_score->Draw();
	m_shoes->Draw();
	m_clock->Draw();
	m_level->Draw();
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

	bomberman->Draw();

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
	for (auto it : m_enemies) {
		it->Draw();
	}
}

void GSPlay::SetBomb2(int pixelWidth, int pixelHeight) {
		BOMB b; b.index = 2; b.xBlock = getWidthBlock_from_WidthPixel(pixelWidth); b.yBlock = getHeightBlock_from_HeightPixel(pixelHeight);
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
void GSPlay::SetExplode2(int pixelWidth, int pixelHeight) {	//bắn tia lửa
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
					if (abs(bombstatus.front().yBlock - bombstatus.back().yBlock) <= FIRE_LENGTH) {
						//cùng 1 hàng và cách nhau chưa tới FIRE_LENGTH theo chiều ngang
						finishBomb1Time = 0.0000001;
					}
				}
				if (bombstatus.front().yBlock == bombstatus.back().yBlock) {	//cùng 1 hàng
					if (abs(bombstatus.front().xBlock - bombstatus.back().xBlock) <= FIRE_LENGTH) {
						//cùng 1 cột và cách nhau chưa tới FIRE_LENGTH theo chiều dọc
						finishBomb1Time = 0.0000001;
					}
				}



			}
		}
		//
	}
	hasFire2BeenFinished = 0;	//tia lửa của bom 2 bắt đầu xuất hiện
	//quả bom thứ 2 đã nổ nghĩa là trong túi tăng từ 0 lên 1 quả bom
	//MAXBOMB++;	//1
	bombstatus.pop();
};	//nổ bom+
void GSPlay::FinishBomb2(float deltaTime) {	//kick nổ bom 2

	if (finishBomb2Time > 0)
		finishBomb2Time -= 1 * deltaTime;
	if (finishBomb2Time < 0 && hasBomb2BeenFinished == 0) {	//nếu hết thời gian mà bom chưa nổ thì kick nổ quả bom
		ResourceManagers::GetInstance()->PlaySound("exploded", false);
		SetExplode2(m_bombs2[0]->Get2DPosition().x, m_bombs2[0]->Get2DPosition().y);
		m_bombs2.erase(m_bombs2.begin() + 0);
		hasBomb2BeenFinished = 1;
	}
};	//nổ bom-
void GSPlay::FinishExplode2(float deltaTime) {
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
void GSPlay::HasBombermanBeenFired(float deltaTime) {	//kiểm tra xem người chơi có chạm vào tia lửa hay ko
	if (!m_fires.empty()) {
		int myPositionBlockWidth = getWidthBlock_from_WidthPixel((bomberman->Get2DPosition().x));
		int myPositionBlockHeight = getHeightBlock_from_HeightPixel((bomberman->Get2DPosition().y));
		for (int i = 0; i < m_fires.size(); i++) {
			int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_fires[i]->Get2DPosition().x));
			int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_fires[i]->Get2DPosition().y));
			if (myPositionBlockWidth == enemyPositionBlockWidth && myPositionBlockHeight == enemyPositionBlockHeight) {
				DiedBomberman(deltaTime);
				break;
			}
		}
	}

	if (!m_fires2.empty()) {
		int myPositionBlockWidth = getWidthBlock_from_WidthPixel((bomberman->Get2DPosition().x));
		int myPositionBlockHeight = getHeightBlock_from_HeightPixel((bomberman->Get2DPosition().y));
		for (int i = 0; i < m_fires2.size(); i++) {
			int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_fires2[i]->Get2DPosition().x));
			int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_fires2[i]->Get2DPosition().y));
			if (myPositionBlockWidth == enemyPositionBlockWidth && myPositionBlockHeight == enemyPositionBlockHeight) {
				DiedBomberman(deltaTime);
				break;
			}
		}
	}
}

//16. kiểm tra xem enemies có chạm vào tia lửa hay ko
void GSPlay::HasEnemiesBeenFired(float deltaTime) {	//kiểm tra xem enemies có chạm vào tia lửa hay ko
	if (!m_fires.empty()) {
		for (int i = 0; i < m_fires.size(); i++) {		//duyệt từng ô fire xem có con enemy nào ở đó ko
			int firePositionBlockWidth = getWidthBlock_from_WidthPixel((m_fires[i]->Get2DPosition().x));
			int firePositionBlockHeight = getHeightBlock_from_HeightPixel((m_fires[i]->Get2DPosition().y));
			int sumVenemies = 0;
			for (int enemy_index = 0; enemy_index < m_enemies.size(); enemy_index++) {
				sumVenemies += Venemies[enemy_index];
				int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_enemies[enemy_index]->Get2DPosition().x));
				int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_enemies[enemy_index]->Get2DPosition().y));
				if (enemyPositionBlockWidth == firePositionBlockWidth && enemyPositionBlockHeight == firePositionBlockHeight) {
					Venemies[enemy_index] = 0;
					m_coins->setText("x : " + to_string(++killedEnemies));
					m_score->setText("Score: "+to_string(killedEnemies) + "0");
					m_enemies[enemy_index]->Set2DPosition(50 * (enemy_index + 9), 0);	//INDEX = 0 VS = 1 CÓ VẺ NHÌN GIỐNG NHAU
					if (Venemies[0] < INITIAL_SPEED) {
						recoverVEnemies();
					}

				}
			}
			if (sumVenemies == 0) { //tất cả enemies đã bị tiêu diệt hết
				hasAllEnemiesBeKilled = 1;
			}
			else hasAllEnemiesBeKilled = 0;	//vẫn còn enemies trên màn hình
		}
	}

	if (!m_fires2.empty()) {
		for (int i = 0; i < m_fires2.size(); i++) {		//duyệt từng ô fire2 xem có con enemy nào ở đó ko
			int firePositionBlockWidth = getWidthBlock_from_WidthPixel((m_fires2[i]->Get2DPosition().x));
			int firePositionBlockHeight = getHeightBlock_from_HeightPixel((m_fires2[i]->Get2DPosition().y));
			int sumVenemies = 0;
			for (int enemy_index = 0; enemy_index < m_enemies.size(); enemy_index++) {
				sumVenemies += Venemies[enemy_index];
				int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_enemies[enemy_index]->Get2DPosition().x));
				int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_enemies[enemy_index]->Get2DPosition().y));
				if (enemyPositionBlockWidth == firePositionBlockWidth && enemyPositionBlockHeight == firePositionBlockHeight) {
					Venemies[enemy_index] = 0;
					m_coins->setText("x : " + to_string(++killedEnemies));
					m_score->setText("Score: " + to_string(killedEnemies) + "0");
					m_enemies[enemy_index]->Set2DPosition(50 * (enemy_index + 9), 0);//INDEX = 0 VS = 1 CÓ VẺ NHÌN GIỐNG NHAU
					if (Venemies[0] < INITIAL_SPEED) {
						recoverVEnemies();
					}
				}
			}
			if (sumVenemies == 0) { //tất cả enemies đã bị tiêu diệt hết
				hasAllEnemiesBeKilled = 1;
			}
			else hasAllEnemiesBeKilled = 0;	//vẫn còn enemies trên màn hình
		}
	}
}

void GSPlay::recoverVEnemies(){
		for (int i = 0; i < Venemies.size(); i++) {
			if (Venemies[i] != 0) {
				Venemies[i] = INITIAL_SPEED;
			}
		}
}

//17. kiểm tra xem người chơi có chạm vào enemies hay ko
void GSPlay::HasBombermanBeenTouchedByEnemies(float deltaTime) {//kiểm tra xem người chơi có chạm vào enemies hay ko
	
	int myPositionBlockWidth = getWidthBlock_from_WidthPixel((bomberman->Get2DPosition().x));
	int myPositionBlockHeight = getHeightBlock_from_HeightPixel((bomberman->Get2DPosition().y));
	for (int i = 0; i < m_enemies.size();i++) {
		int enemyPositionBlockWidth = getWidthBlock_from_WidthPixel((m_enemies[i]->Get2DPosition().x));
		int enemyPositionBlockHeight = getHeightBlock_from_HeightPixel((m_enemies[i]->Get2DPosition().y));
		if (myPositionBlockWidth == enemyPositionBlockWidth && myPositionBlockHeight == enemyPositionBlockHeight) {
			DiedBomberman(deltaTime);
			break;
		}
	}
	
};

//18. Cho Bomberman biến mất khỏi màn hình khi bị bom nổ hoặc chạm vào tia lửa
void GSPlay::DiedBomberman(float deltaTime){
	ResourceManagers::GetInstance()->PauseSound("Level1/LevelBGM" + to_string(level));
	ResourceManagers::GetInstance()->PlaySound("death", false);
	m_skull->Set2DPosition(bomberman->Get2DPosition().x, bomberman->Get2DPosition().y);
	bomberman->Set2DPosition(getWidthPixel_from_WidthBlock((100)), getHeightPixel_from_HeightBlock((100)));
	m_time += deltaTime;
}

//19. Chơi lại màn chơi sau khi chết
void GSPlay::PlayAgain(float deltaTime) {
	if (m_time > 0)	//Khi hàm DiedBomberman đc gọi 1 lần thì hàm này sẽ được kích hoạt
	{
		m_time += deltaTime;
		if (m_time > 4.5) {
			GameStateMachine::GetInstance()->PopState();
			GameStateMachine::GetInstance()->PopState();
			GameStateMachine::GetInstance()->ChangeState(StateTypes::STATE_StageGameStart);
			m_time = 0;
		}
	}
}

//20. Đặt bom vào ô gạch chứa cửa
void GSPlay::CheckBreakDoor() {
	if (HasDoorBeenExplored == 0) {	//nếu cửa chưa được tìm ra
		m_door->Set2DPosition(getWidthPixel_from_WidthBlock(doorWidthByBlock), getHeightPixel_from_HeightBlock(doorHeightByBlock));
		HasDoorBeenExplored = 1;
	}
}

void GSPlay::CheckBreakClock() {
	if (HasClockBeenExplored == 0) {	//nếu đồng hồ chưa được tìm ra
		cout << "Đã tìm thấy đồng hồ\n";
		m_clock->Set2DPosition(getWidthPixel_from_WidthBlock(clockWidthByBlock), getHeightPixel_from_HeightBlock(clockHeightByBlock));
		HasClockBeenExplored = 1;
	}
}

void GSPlay::CheckBreakShoes() {
	if (HasShoesBeenExplored == 0) {	//nếu giày chưa được tìm ra
		cout << "Đã tìm thấy giày\n";
		m_shoes->Set2DPosition(getWidthPixel_from_WidthBlock(shoesWidthByBlock), getHeightPixel_from_HeightBlock(shoesHeightByBlock));
		HasShoesBeenExplored = 1;
	}
}

//21. Đặt bom vào cửa là enemies hồi sinh
void GSPlay::BreakDoor(int blockWidth, int blockHeight) {
	for (int i = 0; i < Venemies.size(); i++) {
		if (Venemies[i] == 0) {	//hồi sinh những enemies đã bị tiêu diệt
			Venemies[i] = INITIAL_SPEED;
			//m_enemies[i]->Set2DPosition(getWidthPixel_from_WidthBlock(27), getHeightPixel_from_HeightBlock(13));
		}
	}
}

//22. Diệt hết enemies, vào cửa để sang stage tiếp theo
void GSPlay::NextLevel() {
	if (getWidthBlock_from_WidthPixel(bomberman->Get2DPosition().x) == doorWidthByBlock && getHeightBlock_from_HeightPixel(bomberman->Get2DPosition().y) == doorHeightByBlock) {
		ResourceManagers::GetInstance()->PauseSound("Level1/LevelBGM" + to_string(level));	//vào cửa thì dừng nhạc
		ResourceManagers::GetInstance()->PlaySound("stage_clear");
	}
}