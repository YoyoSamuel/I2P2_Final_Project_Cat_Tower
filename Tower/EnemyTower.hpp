#ifndef ENEMYTOWER_HPP
#define ENEMYTOWER_HPP
#include "Tower.hpp"

class Enemytower: public Tower {
public:
	static const int hp = 1000;
    Enemytower(float x, float y, int MapID);
};
#endif // ENEMYTOWER_HPP