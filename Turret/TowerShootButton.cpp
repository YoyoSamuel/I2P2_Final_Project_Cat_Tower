#include <allegro5/color.h>
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "TowerShootButton.hpp"

PlayScene* TowerButton::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
TowerButton::TowerButton(std::string img, std::string imgIn, float x, float y, int CoolTime) :
	ImageButton(img, imgIn, x, y), CoolTime(CoolTime) {
}
void TowerButton::Update(float deltaTime) {
	ImageButton::Update(deltaTime);
	if (getPlayScene()->GetTime() >= CoolTime) {
		Enabled = true;
		this->bmp = Engine::Resources::GetInstance().GetBitmap("play/ShootButtonE.png", Size.x, Size.y);
		TowerButton::Draw();
	} else {
		Enabled = false;
		this->bmp = Engine::Resources::GetInstance().GetBitmap("play/ShootButton.png", Size.x, Size.y);
		TowerButton::Draw();
	}
}
void TowerButton::Draw() const {
	ImageButton::Draw();
}
