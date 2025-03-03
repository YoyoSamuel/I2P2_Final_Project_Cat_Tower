#ifndef CATTOWER_HPP
#define CATTOWER_HPP
#include "Tower.hpp"

class Cattower: public Tower {
public:
	static const int hp = 1000;
    Cattower(float x, float y);
};
#endif // CATTOWER_HPP
