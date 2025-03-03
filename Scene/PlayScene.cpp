#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>

#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "UI/Component/Label.hpp"
#include "Turret/LaserTurret.hpp"
#include "Turret/MachineGunTurret.hpp"
#include "Turret/MissileTurret.hpp"
#include "Turret/ThreeBulletTurret.hpp"
#include "UI/Animation/Plane.hpp"
#include "PlayScene.hpp"
#include "Engine/Resources.hpp"
#include "Enemy/Dog.hpp"
#include "Turret/TurretButton.hpp"
#include "Turret/TowerShootButton.hpp"
#include "Turret/CatButton.hpp"
#include "Character/Character.hpp"
#include "Character/Basiccat.hpp"
#include "Character/Tankcat.hpp"
#include "Character/Basiccat.hpp"
#include "Character/Fightercat.hpp"
#include "Turret/Pocket.hpp"
#include "Tower/CatTower.hpp"
#include "Tower/EnemyTower.hpp"
#include "Tower/Tower.hpp"


bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, MapHeight - 4);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 4);
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
									ALLEGRO_KEY_LEFT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_RIGHT,
									ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_ENTER };
Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	now = time(0);
	preshoot = time(0);
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	lives = 10;
	MoneyLimit = 150;
	//CatOgary = -1;
	cha = nullptr;
	money = 100;
	SpeedMult = 1;
	explosionx = 200;
	count = 0;
	shoot = false;
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
	AddNewObject(new Engine::Image("play/true_sky.jpg", 0, 0, 0, 0));
	AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(BadtowerGroup = new Group());
	AddNewObject(CatowerGroup = new Group());
	AddNewObject(EnemyGroup = new Group());
	AddNewObject(CharacterGroup = new Group());
	AddNewObject(BulletGroup = new Group());
	AddNewObject(EffectGroup = new Group());
	AddNewObject(Explosion = new Group());
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ReadEnemyWave();

	// Path Finding.
	mapDistance = CalculateBFSDistance();
	for(auto it : mapDistance){
		std::vector<int> temp;
		if(it[0] == -1){
			for(auto it2 : it){
				temp.push_back(it2);
			}
		}
		else{
			for(auto it2 : it){
				temp.push_back(MapWidth - it2 - 1);
			}
		}
		revmapDistance.push_back(temp);
	}

	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);

	// Preload Lose Scene
	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
	bgmId = AudioHelper::PlayBGM("play.ogg");
	CatowerGroup->AddNewObject(catt = new Cattower(w*11/12-250, 530));
	BadtowerGroup->AddNewObject(badd = new Enemytower(w*1/12, 530, MapId));
}
void PlayScene::Terminate() {
	AudioHelper::StopBGM(bgmId);
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	// If we use deltaTime directly, then we might have Bullet-through-paper problem.
	// Reference: Bullet-Through-Paper
	if(catt->GetHP() < 0) Engine::GameEngine::GetInstance().ChangeScene("win");
	if(badd->GetHP() < 0) Engine::GameEngine::GetInstance().ChangeScene("lose");
	now = time(0);
	count++;
	if(count%6 == 0 && money < MoneyLimit)
		EarnMoney(1);
	CatowerHP->Text = std::to_string((int)catt->GetHP()) + std::string(" / ") + std::to_string((int)1000);
	EnemytowerHP->Text = std::to_string((int)badd->GetHP()) + std::string(" / ") + std::to_string((int)MapId == 1?1000:2000);
	if (SpeedMult == 0)
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;
	// Calculate danger zone.
	std::vector<float> reachEndTimes;
	for (auto& it : EnemyGroup->GetObjects()) {
		reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
	}
	// Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
	std::sort(reachEndTimes.begin(), reachEndTimes.end());
	float newDeathCountDown = -1;
	int danger = lives;
	for (auto& it : reachEndTimes) {
		if (it <= DangerTime) {
			danger--;
			if (danger <= 0) {
				// Death Countdown
				float pos = DangerTime - it;
				if (it > deathCountDown) {
					// Restart Death Count Down BGM.
					AudioHelper::StopSample(deathBGMInstance);
					//if (SpeedMult != 0)
						//deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
				}
				float alpha = pos / DangerTime;
				alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
				dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
				newDeathCountDown = it;
				break;
			}
		}
	}
	deathCountDown = newDeathCountDown;
	if (SpeedMult == 0)
		AudioHelper::StopSample(deathBGMInstance);
	if (deathCountDown == -1 && lives > 0) {
		AudioHelper::StopSample(deathBGMInstance);
		dangerIndicator->Tint.a = 0;
	}
	if (SpeedMult == 0)
		deathCountDown = -1;
	for (int i = 0; i < SpeedMult; i++) {
		IScene::Update(deltaTime);
		// Check if we should create new enemy.
		ticks += deltaTime;
		if (enemyWaveData.empty()) {
			if (EnemyGroup->GetObjects().empty()) {
				// Free resources.
				/*
				delete TileMapGroup;
				delete GroundEffectGroup;
				delete DebugIndicatorGroup;
				delete TowerGroup;
				delete EnemyGroup;
				delete BulletGroup;
				delete EffectGroup;
				delete UIGroup;
				delete imgTarget;
				*/
				std::cout << "You Win" << std::endl;
				SaveScore("../Resource/scoreboard.txt");
				Engine::GameEngine::GetInstance().ChangeScene("win");
			}
			continue;
		}
		auto current = enemyWaveData.front();
		if (ticks < current.second)
			continue;
		ticks -= current.second;
		enemyWaveData.pop_front();
		const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
		Enemy* enemy;
		switch (current.first) {
		case 1:
			EnemyGroup->AddNewObject(enemy = new Dog(SpawnCoordinate.x+155, SpawnCoordinate.y));
			break;
		case 2:
			EnemyGroup->AddNewObject(enemy = new Dog(SpawnCoordinate.x+100, SpawnCoordinate.y));
			break;
		case 3:
			EnemyGroup->AddNewObject(enemy = new Dog(SpawnCoordinate.x+100, SpawnCoordinate.y));
			break;
		case 4:
			EnemyGroup->AddNewObject(enemy = new Dog(SpawnCoordinate.x+100, SpawnCoordinate.y));
			break;
		default:
			continue;
		}
		enemy->UpdatePath(mapDistance);
		// Compensate the time lost.
		enemy->Update(ticks);
	}
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}
void PlayScene::Draw() const {
	IScene::Draw();
	if (DebugMode) {
		// Draw reverse BFS distance on all reachable blocks.
		for (int i = 0; i < MapHeight; i++) {
			for (int j = 0; j < MapWidth; j++) {
				if (mapDistance[i][j] != -1) {
					// Not elegant nor efficient, but it's quite enough for debugging.
					Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
					label.Anchor = Engine::Point(0.5, 0.5);
					label.Draw();
				}
			}
		}
	}
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel turret construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget->Visible = false;
		return;
	}
	imgTarget->Visible = true;
	imgTarget->Position.x = x * BlockSize;
	imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);
	if (!imgTarget->Visible)
		return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if (button & 1) {
		if (mapState[y][x] != TILE_OCCUPIED) {
			if (!preview)
				return;
			// Check if valid.
			/*
			if (!CheckSpaceValid(x, y)) {
				Engine::Sprite* sprite;
				GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
				sprite->Rotation = 0;
				return;
			}*/
			// Purchase.
			EarnMoney(-preview->GetPrice());
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
			UIGroup->RemoveObject(preview->GetObjectIterator());
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
			preview->Preview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;

			mapState[y][x] = TILE_OCCUPIED;
			OnMouseMove(mx, my);
		}
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB) {
		DebugMode = !DebugMode;
	}
	else {
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size()) {
			auto it = keyStrokes.begin();
			for (int c : code) {
				if (!((*it == c) ||
					(c == ALLEGRO_KEYMOD_SHIFT &&
					(*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}
			EffectGroup->AddNewObject(new Plane());
			EarnMoney(10000);
		}
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for MachineGunTurret.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		// Hotkey for LaserTurret.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E) {
		// Hotkey for MissileTurret.
		UIBtnClicked(2);
	}
	// TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
	else if (keyCode == ALLEGRO_KEY_R) {
		// Hotkey for ThreeBulletTurret.
		UIBtnClicked(3);
	}
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}


void PlayScene::Hit() {
	lives--;
	/*UILives->Text = std::string("Life ") + std::to_string(lives);
	if (lives <= 0) {
		Engine::GameEngine::GetInstance().ChangeScene("lose");
	}*/
}
int PlayScene::GetTime() const { return now-preshoot; }
int PlayScene::Getcount() const { return count; }
int PlayScene::GetMoney() const { return money; }
void PlayScene::EarnMoney(int money) {
	this->money += money;
	UIMoney->Text = std::string("$ ") + std::to_string(this->money) + std::string(" / ") + std::to_string(MoneyLimit);
}
void PlayScene::ReadMap() {
	std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<bool> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
		switch (c) {
		case '0': mapData.push_back(false); break;
		case '1': mapData.push_back(true); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const int num = mapData[i * MapWidth + j];
			mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
			if (num)
				if(i == MapHeight-3) TileMapGroup->AddNewObject(new Engine::Image("play/top_dirt-2.png", j * BlockSize, i * BlockSize - 12, 0, 0));
				else TileMapGroup->AddNewObject(new Engine::Image("play/dirt-4.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
			else
				TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
		}
	}
}
void PlayScene::ReadEnemyWave() {
    // TODO: [HACKATHON-3-BUG] (3/5): Trace the code to know how the enemies are created.
    // TODO: [HACKATHON-3-BUG] (3/5): There is a bug in these files, which let the game only spawn the first enemy, try to fix it.
    std::string filename = std::string("Resource/enemy") + std::to_string(MapId) + ".txt";
	// Read enemy file.
	float type, wait, repeat;
	enemyWaveData.clear();
	std::ifstream fin(filename);
	while (fin >> type && fin >> wait && fin >> repeat) {
		for (int i = 0; i < repeat; i++)
			enemyWaveData.emplace_back(type, wait);
	}
	fin.close();
}
void PlayScene::ConstructUI() {
	// Background
	//UIGroup->AddNewObject(new Engine::Image("play/sa.png", 1280, 350, 320, 832));
	// Text
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	UIGroup->AddNewObject(new Engine::Image("play/sa.png", w*11/12-30, h*7/8+7, 0, 0, 0.5, 0.5));
	UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
	UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money) + std::string(" / ") + std::to_string(MoneyLimit), "pirulen.ttf", 24, 1294, 48));
	//UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
	UIGroup->AddNewObject(CatowerHP = new Engine::Label(std::to_string((int)1000) + std::string(" / ") + std::to_string((int)1000), "pirulen.ttf", 17, 1175, 370));
	UIGroup->AddNewObject(EnemytowerHP = new Engine::Label(std::to_string((int)1000) + std::string(" / ") + std::to_string((int)1000), "pirulen.ttf", 17, 200, 370));
	CatButton* cbtn;
	TowerButton* tobtn;
	// Cat Button 1
	cbtn = new CatButton("play/button-cat.png", "play/button-cat-un.png"
        , 200, 650, Basiccat::Price);
    cbtn->SetOnClickCallback(std::bind(&PlayScene::CatBtnClicked, this, 1));
    UIGroup->AddNewControlObject(cbtn);
	// Cat Button 2
    cbtn = new CatButton("play/button-tankcat.png", "play/button-tankcat-un.png"
        , 400, 650, Tankcat::Price);
    cbtn->SetOnClickCallback(std::bind(&PlayScene::CatBtnClicked, this, 2));
    UIGroup->AddNewControlObject(cbtn);
	// Cat Button 3
    cbtn = new CatButton("play/button-Fightercat.png", "play/button-Fightercat-un.png"
        , 600, 650, Fightercat::Price);
    cbtn->SetOnClickCallback(std::bind(&PlayScene::CatBtnClicked, this, 3));
    UIGroup->AddNewControlObject(cbtn);
	// Cat Button 4
	pocket = new Pocket("play/pocket.png", "play/pocket-un.png", 0, 720, 100);
	pocket->SetOnClickCallback(std::bind(&PlayScene::CatBtnClicked, this, 4));
	UIGroup->AddNewControlObject(pocket);
	// CatToTower Button
	tobtn = new TowerButton("play/ShootButtonE.png", "play/ShootButtonE.png",
		1350, 650, 10);
	tobtn->SetOnClickCallback(std::bind(&PlayScene::ShootButtonClicked, this, 4));
	UIGroup->AddNewControlObject(tobtn);
	int shift = 135 + 25;
	dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
	dangerIndicator->Tint.a = 0;
	UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::ShootButtonClicked(int id)
{
	preshoot = time(0);
	shoot = true;
	for (auto& it : EnemyGroup->GetObjects())
	{
		Enemy* enemy = dynamic_cast<Enemy*>(it);
		enemy->Hit(10);
	}
}

void PlayScene::CatBtnClicked(int id) {
        const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
        if (id == 1 && money >= Basiccat::Price)
        {
            Basiccat* cat = new Basiccat(1200, 610);
            EarnMoney(-Basiccat::Price);
            CharacterGroup->AddNewObject(cat);
            cat->UpdatePath(revmapDistance);
            // Compensate the time lost.
            cat->Update(ticks);
        }
        else if (id == 2 && money >= Tankcat::Price)
        {
            Tankcat* tankcat = new Tankcat(1200, 610);
            EarnMoney(-Tankcat::Price);
            CharacterGroup->AddNewObject(tankcat);
            tankcat->UpdatePath(revmapDistance);
            // Compensate the time lost.
            tankcat->Update(ticks);
        }
        else if (id == 3 && money >= Fightercat::Price)
        {
            Fightercat* fightercat = new Fightercat(1200, 610);
            EarnMoney(-Fightercat::Price);
            CharacterGroup->AddNewObject(fightercat);
            fightercat->UpdatePath(revmapDistance);
            // Compensate the time lost.
            fightercat->Update(ticks);
        }
		else if (id == 4 && money >= pocket->money)
		{
			EarnMoney(-pocket->money);
			pocket->money += 50;
			MoneyLimit *= 2;
		}
}

void PlayScene::UIBtnClicked(int id) {
	if (preview)
	UIGroup->RemoveObject(preview->GetObjectIterator());
    // TODO: [CUSTOM-TURRET]: On callback, create the turret.
	if (id == 0 && money >= MachineGunTurret::Price)
		preview = new MachineGunTurret(10, 0);
	else if (id == 1 && money >= LaserTurret::Price)
		preview = new LaserTurret(0, 0);
	else if (id == 2 && money >= MissileTurret::Price)
		preview = new MissileTurret(0, 0);
	else if (id == 3 && money >= ThreeBulletTurret::Price)
		preview = new ThreeBulletTurret(0, 0);
	if (!preview)
		return;
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
	preview->Preview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}
//622
/*bool PlayScene::CheckSpaceValid(int x, int y) {
	if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
		return false;
	auto map00 = mapState[y][x];
	mapState[y][x] = TILE_OCCUPIED;
	std::vector<std::vector<int>> map = CalculateBFSDistance();
	mapState[y][x] = map00;
	if (map[0][0] == -1)
		return false;
	for (auto& it : EnemyGroup->GetObjects()) {
		Engine::Point pnt;
		pnt.x = floor(it->Position.x / BlockSize);
		pnt.y = floor(it->Position.y / BlockSize);
		if (pnt.x < 0) pnt.x = 0;
		if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
		if (pnt.y < 0) pnt.y = 0;
		if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
		if (map[pnt.y][pnt.x] == -1)
			return false;
	}
	// All enemy have path to exit.
	mapState[y][x] = TILE_OCCUPIED;
	mapDistance = map;
	for (auto& it : EnemyGroup->GetObjects())
		dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
	return true;
}*/
void PlayScene::SaveScore(const std::string filename)
{
 	std::ofstream file;
	file.open(filename, std::ios::app);
	int score = money + lives * 600;
	if(!file.is_open())
		std::cout<<"wrong";
	std::string scoreenter = std::to_string(score);
	file << "name " << scoreenter << "\n";
	std::cout << "save success\n";
	file.close();
	// Can't open file.
}

std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
	// Reverse BFS to find path.
	std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
	std::queue<Engine::Point> que;
	// Push end point.
	// BFS from end point.
	for (int i = MapWidth - 1; i >= 0; i--) map[MapHeight - 4][i] = MapWidth - i - 1;
	return map;
}