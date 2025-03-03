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
#include "Tower.hpp"

Tower::Tower(std::string img, float x, float y, float radius, float hp, float originalhp) :
Sprite(img, x, y), hp(hp), name(img), originalhp(originalhp) { CollisionRadius = radius; }
PlayScene *Tower::getPlayScene() { return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene()); }

void Tower::Update(float deltaTime) {
	Sprite::Update(deltaTime);
}
void Tower::Draw() const {
	// if (Preview) {
	// 	al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(0, 255, 0, 50));
	// }
	Sprite::Draw();
}
float Tower::GetHP() const { return hp; }
void Tower::Hit(float damage) {
	hp -= damage;
	if (hp <= 0) {
			// Remove all turret's reference to target.
			Target = nullptr;
			//623getPlayScene()->CharacterGroup->RemoveObject(objectIterator);
			AudioHelper::PlayAudio("explosion.wav");
	}
}