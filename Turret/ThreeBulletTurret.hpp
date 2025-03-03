#ifndef THREEBULLETTURRET_HPP
#define THREEBULLETTURRET_HPP
#include "Turret.hpp"

class ThreeBulletTurret: public Turret {
public:
	static const int Price;
    ThreeBulletTurret(float x, float y);
	void CreateBullet() override;
};
#endif // THREEBULLETTURRET_HPP