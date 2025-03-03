#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <random>

#include "EnemyTower.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"

// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.


Enemytower::Enemytower(float x, float y, int MapID) :
    Tower(MapID==1?"play/Enemytower.png":"play/Enemytower-2.png", x, MapID==1?y:y-50, 100, MapID==1?1000:2000, MapID==1?1000:2000) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}


