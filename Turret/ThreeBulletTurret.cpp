#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Bullet/ThreeBullet.hpp"
#include "ThreeBulletTurret.hpp"
#include "Scene/PlayScene.hpp"
#include "Engine/Point.hpp"

const int ThreeBulletTurret::Price = 2;
ThreeBulletTurret::ThreeBulletTurret(float x, float y) :
	Turret("play/tower-base.png", "play/turret-8.png", x, y, 250, Price, 0.6) {
	// Move center downward, since we the turret head is slightly biased upward.
	Anchor.y += 8.0f / GetBitmapHeight();
}
void ThreeBulletTurret::CreateBullet() {
	Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
	float rotation = atan2(diff.y, diff.x);
	Engine::Point normalized = diff.Normalize();
	Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
	// Change bullet position to the front of the gun barrel.
	getPlayScene()->BulletGroup->AddNewObject(new ThreeBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
	getPlayScene()->BulletGroup->AddNewObject(new ThreeBullet(Position + normalized * 36 + normal * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new ThreeBullet(Position + normalized * 36 - normal * 15, diff, rotation, this));
	getPlayScene()->BulletGroup->AddNewObject(new ThreeBullet(Position + normalized * 36 + normal * 15, diff, rotation, this));
	AudioHelper::PlayAudio("threebullet.mp3");
}
