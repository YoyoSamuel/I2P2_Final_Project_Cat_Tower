#ifndef TANKCAT_HPP
#define TANKCAT_HPP
#include "Character.hpp"

class Tankcat: public Character {
public:
	static const int Price = 100;
    Tankcat(float x, float y);
    void OnExplode(Enemy* enemy) override;
    void TowerOnExplode(Tower* tower) override;
};
#endif // TANKCAT_HPP
