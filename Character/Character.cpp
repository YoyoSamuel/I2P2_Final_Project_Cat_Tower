#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <utility>
#include <random>
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Collider.hpp"
#include "Tower/Tower.hpp"
#include "Tower/Enemytower.hpp"
#include "Character.hpp"

Character::Character(std::string img, std::string imgm, std::string imgat, float x, float y, float radius, int price, int speed, int hp, float coolDown, float damage) :
	Sprite(img, x, y), price(price), speed(speed), hp(hp), coolDown(coolDown), damage(damage), name(img), namem(imgm), nameat(imgat) { CollisionRadius = radius; }
PlayScene *Character::getPlayScene() { return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene()); }

void Character::Update(float deltaTime) {
	Sprite::Update(deltaTime);
	if (getPlayScene()->Getcount() %100 > 50) {
        this->bmp = Engine::Resources::GetInstance().GetBitmap(name, Size.x, Size.y);
        Character::Draw();
    } else {
        this->bmp = Engine::Resources::GetInstance().GetBitmap(namem, Size.x, Size.y);
        Character::Draw();
    }
	PlayScene* scene = getPlayScene();
	// Can be improved by Spatial Hash, Quad Tree, ...
	// However simply loop through all enemies is enough for this program.
		for (auto& it : scene->EnemyGroup->GetObjects()) {
			Enemy* enemy = dynamic_cast<Enemy*>(it);
			if (!enemy->Visible)
				continue;
			if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
				Velocity.x = 0;
				if(getPlayScene()->Getcount() %120 > 110)
				{
					this->bmp = Engine::Resources::GetInstance().GetBitmap(nameat, Size.x, Size.y);
        			Character::Draw();
					std::cout << "crushTBadTower\n";
					if(getPlayScene()->Getcount() %120 > 118)
					{
						OnExplode(enemy);
						enemy->Hit(damage);
					}

				}
				return;
			}
		}
		for (auto& it : scene->BadtowerGroup->GetObjects()) {
		Tower* enem = dynamic_cast<Tower*>(it);
		if (!enem->Visible)
			continue;
		if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enem->Position, enem->CollisionRadius)) {
			Velocity.x = 0;
			if(getPlayScene()->Getcount() %120 > 110)
			{
				this->bmp = Engine::Resources::GetInstance().GetBitmap(nameat, Size.x, Size.y);
        		Character::Draw();
				if(getPlayScene()->Getcount() %120 > 118)
				{
					TowerOnExplode(enem);
					enem->Hit(damage);
				}

			}
			return;
		}
		}
	// Check if out of boundary.
	if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
		getPlayScene()->BulletGroup->RemoveObject(objectIterator);

	// Pre-calculate the velocity.
	float remainSpeed = speed * deltaTime;
	while (remainSpeed != 0) {
		if (path.empty()) {
			// Reach end point.
			Hit(hp);
			getPlayScene()->Hit();
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
	Sprite::Update(deltaTime);
}
void Character::Draw() const {
	// if (Preview) {
	// 	al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(0, 255, 0, 50));
	// }
	Sprite::Draw();
	if (PlayScene::DebugMode) {
		// Draw target radius.
		al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(0, 0, 255), 2);
	}
}
int Character::GetPrice() const { return price; }
void Character::Hit(float damage) {
	hp -= damage;
	if (hp <= 0) {
			// Remove all turret's reference to target.
			Target = nullptr;
			getPlayScene()->CharacterGroup->RemoveObject(objectIterator);
			AudioHelper::PlayAudio("explosion.wav");
	}
}

void Character::UpdatePath(const std::vector<std::vector<int>>& mapDistance) {
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
		Engine::LOG(Engine::ERROR) << "Character path finding error";
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