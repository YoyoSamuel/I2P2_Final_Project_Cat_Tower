#include <allegro5/base.h>
#include <cmath>
#include <string>
#include <random>

#include "CatTower.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/LaserBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Enemy/Enemy.hpp"

// Use bounding circle to detect collision is for simplicity, pixel-perfect collision can be implemented quite easily,
// and efficiently if we use AABB collision detection first, and then pixel-perfect collision.


Cattower::Cattower(float x, float y) :
    Tower("play/cattower.png", x, y, 100, 1000, 1000) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}


