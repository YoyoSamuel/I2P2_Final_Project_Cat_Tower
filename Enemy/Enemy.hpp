#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <vector>
#include <string>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;
class Character;
class Tower;

class Enemy : public Engine::Sprite {
protected:
 	std::vector<Engine::Point> path;
	float speed;
	bool life;
	std::string name, unname, nameat;
    float damage;
    float coolDown;
    float reload = 0;
    PlayScene* getPlayScene();
    virtual void OnExplode();
	virtual void CharacterOnExplode(Character* character) = 0;
	virtual void TowerOnExplode(Tower* tower) = 0;
public:
	float hp;
	int money;
    float reachEndTime;
	std::list<Turret*> lockedTurrets;
	std::list<Bullet*> lockedBullets;
	std::list<Character*> lockedCharacters;
	Enemy(std::string img, std::string unimg, std::string nameat, float x, float y, float radius, float speed, float hp, int life, int money, float coolDown, float damage);
 	void virtual Hit(float damage);
	void UpdatePath(const std::vector<std::vector<int>>& mapDistance);
	void Update(float deltaTime) override;
	void Draw() const override;
    Character* Target = nullptr;
};
#endif // ENEMY_HPP
