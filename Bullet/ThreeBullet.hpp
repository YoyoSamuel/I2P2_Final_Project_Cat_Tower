#ifndef THREEBULLET_HPP
#define THREEBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
struct Point;
}  // namespace Engine

class ThreeBullet : public Bullet {
public:
	explicit ThreeBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent);
	void OnExplode(Enemy* enemy) override;
};
#endif // THREEBULLET_HPP
