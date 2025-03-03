#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <random>

#include "Fightercat.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"
#include "Tower/Tower.hpp"
// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.


Fightercat::Fightercat(float x, float y) :
    Character("play/Fightercat.png", "play/Fightercat-m.png", "play/Fightercat-m.png", x, y, 30, 350, 10, 100, 0.5, 30) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}

void Fightercat::OnExplode(Enemy* enemy) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-2.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

void Fightercat::TowerOnExplode(Tower* tower) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/explosion-2.png", dist(rng), tower->Position.x, tower->Position.y));
}

