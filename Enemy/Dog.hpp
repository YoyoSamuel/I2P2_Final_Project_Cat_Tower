#ifndef DOG_HPP
#define DOG_HPP
#include "Enemy.hpp"


class Dog : public Enemy {
public:
	Dog(int x, int y);
    void CharacterOnExplode(Character *character) override;
    void TowerOnExplode(Tower* tower) override;
};
#endif // DOG_HPP
