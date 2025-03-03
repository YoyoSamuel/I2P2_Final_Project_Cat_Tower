#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Bullet/Bullet.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy.hpp"
#include "Engine/Resources.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Turret/Turret.hpp"
#include "Character/Character.hpp"
#include "Tower/Tower.hpp"
#include "Tower/CatTower.hpp"
#include "Engine/Collider.hpp"

PlayScene* Enemy::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Enemy::OnExplode() {
	getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
	for (int i = 0; i < 10; i++) {
		// Random add 10 dirty effects.
		getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
	}
}
Enemy::Enemy(std::string img, std::string unimg, std::string nameat, float x, float y, float radius, float speed, float hp, int life, int money, float coolDown, float damage) :
	Engine::Sprite(img, x, y), speed(speed), hp(hp), money(money), life(life), coolDown(coolDown), damage(damage), name(img), unname(unimg), nameat(nameat) {
	CollisionRadius = radius;
	reachEndTime = 0;
}
void Enemy::Hit(float damage) {
	hp -= damage;
	if (hp <= 0) {
			if(getPlayScene()->GetMoney() + money < getPlayScene()->MoneyLimit){
                getPlayScene()->EarnMoney(money);
            }
			OnExplode();
			Target = nullptr;
			getPlayScene()->EnemyGroup->RemoveObject(objectIterator);
			AudioHelper::PlayAudio("explosion.wav");
		}
}


void Enemy::UpdatePath(const std::vector<std::vector<int>>& mapDistance) {
	std::cout << Position.x << "\n";
	int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
	int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
	if (x < 0) x = 0;
	if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
	if (y < 0) y = 0;
	if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
	Engine::Point pos(x, y);
	int num = mapDistance[y][x];
	if (num == -1) {
		num = 0;
		Engine::LOG(Engine::ERROR) << "Enemy path finding error";
	}
	path = std::vector<Engine::Point>(num + 1);
	while (num != 0) {
		std::vector<Engine::Point> nextHops;
		for (auto& dir : PlayScene::directions) {
			int x = pos.x + dir.x;
			int y = pos.y + dir.y;
			if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
				continue;
			nextHops.emplace_back(x, y);
		}
		// Choose arbitrary one.
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
		pos = nextHops[dist(rng)];
		path[num] = pos;
		num--;
	}
	path[0] = PlayScene::EndGridPoint;
}
void Enemy::Update(float deltaTime) {
	// Pre-calculate the velocity.
	//add
	Sprite::Update(deltaTime);
	if (getPlayScene()->Getcount() %100 > 50) {
        this->bmp = Engine::Resources::GetInstance().GetBitmap(name, Size.x, Size.y);
        Enemy::Draw();
    } else {
        this->bmp = Engine::Resources::GetInstance().GetBitmap(unname, Size.x, Size.y);
        Enemy::Draw();
    }
	PlayScene* scene = getPlayScene();
		for (auto& it : scene->CharacterGroup->GetObjects()) {
		Character* enemy = dynamic_cast<Character*>(it);
		if (!enemy->Visible)
			continue;
		if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
			Velocity.x = 0;
			if(getPlayScene()->Getcount() %120 > 110)
			{
				this->bmp = Engine::Resources::GetInstance().GetBitmap(nameat, Size.x, Size.y);
        		Enemy::Draw();
				//std::cout << "crushTBadTower\n";
				if(getPlayScene()->Getcount() %120 > 118)
				{
					CharacterOnExplode(enemy);
					enemy->Hit(damage);
				}
			}
		}
		}
		for (auto& it : scene->CatowerGroup->GetObjects()) {
		Tower* enem = dynamic_cast<Tower*>(it);
		if (!enem->Visible)
			continue;
		if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enem->Position, enem->CollisionRadius)) {
			Velocity.x = 0;
			if(getPlayScene()->Getcount() %120 > 110)
			{
				this->bmp = Engine::Resources::GetInstance().GetBitmap(nameat, Size.x, Size.y);
        		Enemy::Draw();
				if(getPlayScene()->Getcount() %120 > 118)
				{
					TowerOnExplode(enem);
					enem->Hit(damage);
				}
			}
			return;
		}
		}
	//adde
	
	float remainSpeed = speed * deltaTime;
	while (remainSpeed != 0) {
		if (path.empty()) {
			// Reach end point.
			Hit(hp);
			//getPlayScene()->Hit();
			reachEndTime = 0;
			return;
		}
		Engine::Point target = path.back() * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
		Engine::Point vec = target - Position;
		// Add up the distances:
		// 1. to path.back()
		// 2. path.back() to border
		// 3. All intermediate block size
		// 4. to end point
		reachEndTime = (vec.Magnitude() + (path.size() - 1) * PlayScene::BlockSize - remainSpeed) / speed;
		Engine::Point normalized = vec.Normalize();
		if (remainSpeed - vec.Magnitude() > 0) {
			Position = target;
			path.pop_back();
			remainSpeed -= vec.Magnitude();
		}
		else {
			Velocity = normalized * remainSpeed / deltaTime;
			remainSpeed = 0;
		}
	}
	Rotation = 0;
}
void Enemy::Draw() const {
	Sprite::Draw();
	if (PlayScene::DebugMode) {
		// Draw collision radius.
		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
	}
}




