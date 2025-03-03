#ifndef FIGHTERCAT_HPP
#define FIGHTERCAT_HPP
#include "Character.hpp"

class Fightercat: public Character {
public:
	static const int Price = 350;
    Fightercat(float x, float y);
    void OnExplode(Enemy* enemy) override;
    void TowerOnExplode(Tower* tower) override;
};
#endif // FIGHTERCAT_HPP
