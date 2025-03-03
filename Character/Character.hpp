#ifndef CHARACTER_HPP
#define CHARACTER_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <vector>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;
class Tower;
using std::list;
using std::vector;

class Character: public Engine::Sprite {
protected:
    vector<Engine::Point> path;
	bool life;
    int price;
    float damage;
	float hp;
    float speed;
    float coolDown;
    float reload = 0;
    float reachEndTime;
    PlayScene* getPlayScene();

    virtual void TowerOnExplode(Tower* tower) = 0;
    // Reference: Design Patterns - Factory Method.
public:
    bool Enabled = true;
    bool Preview = false;
    virtual void OnExplode(Enemy* enemy) = 0;
    std::string name, namem, nameat; 
    Enemy* Target = nullptr;
    Character(std::string img, std::string imgm, std::string imgat, float x, float y, float radius, int price, int speed, int hp, float coolDown, float damage);
    void virtual Hit(float damage);
    void Update(float deltaTime) override;
    void UpdatePath(const vector<vector<int>>& mapDistance);
    void Draw() const override;
	int GetPrice() const;
};
#endif // CHARACTER_HPP
