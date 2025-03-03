#include <string>
#include <random>
#include "Dog.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Character/Character.hpp"
#include "Tower/Tower.hpp"


Dog::Dog(int x, int y) : Enemy("play/Dog.png", "play/Dog-m.png", "play/Dog-at.png", x, y, 50, 50, 40, 1, 10, 10, 8)
{
	// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
	// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.
}

void Dog::CharacterOnExplode(Character* character) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/explosion-2.png", dist(rng), character->Position.x, character->Position.y));
}
void Dog::TowerOnExplode(Tower* tower) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
	getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/explosion-2.png", dist(rng), tower->Position.x, tower->Position.y));
}