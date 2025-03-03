#ifndef TOWER_HPP
#define TOWER_HPP
#include <allegro5/base.h>
#include <list>
#include <string>
#include <vector>

#include "Engine/Sprite.hpp"

class Enemy;
class PlayScene;
using std::list;
using std::vector;

class Tower: public Engine::Sprite {
protected:
    vector<Engine::Point> path;
	bool life;
    PlayScene* getPlayScene();
    // Reference: Design Patterns - Factory Method.
public:
	float hp;
    float originalhp;
    bool Enabled = true;
    bool Preview = false;
    std::string name; 
    Enemy* Target = nullptr;
    Tower(std::string img, float x, float y, float radius, float hp, float Originalhp);
    void virtual Hit(float damage);
    void Update(float deltaTime) override;
    void Draw() const override;
	float GetHP() const;
};
#endif // TOWER_HPP
