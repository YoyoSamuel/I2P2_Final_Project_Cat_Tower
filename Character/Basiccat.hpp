#ifndef BASICCAT_HPP
#define BASICCAT_HPP
#include "Character.hpp"

class Basiccat: public Character {
public:
	static const int Price = 50;
    Basiccat(float x, float y);
    void OnExplode(Enemy* enemy) override;
    void TowerOnExplode(Tower* tower) override;
};
#endif // BASICCAT_HPP
