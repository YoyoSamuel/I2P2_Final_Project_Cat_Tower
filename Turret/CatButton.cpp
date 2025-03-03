#include <allegro5/color.h>
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include "CatButton.hpp"

PlayScene* CatButton::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
CatButton::CatButton(std::string img, std::string imgIn,  float x, float y, int money) :
	ImageButton(img, img, x, y), money(money), name(img), unname(imgIn) {
}
void CatButton::Update(float deltaTime) {
	ImageButton::Update(deltaTime);
	if (getPlayScene()->GetMoney() >= money) {
		Enabled = true;
		this->bmp = Engine::Resources::GetInstance().GetBitmap(name, Size.x, Size.y);
		CatButton::Draw();
	} else {
		Enabled = false;
		this->bmp = Engine::Resources::GetInstance().GetBitmap(unname, Size.x, Size.y);
		CatButton::Draw();
	}
}
void CatButton::Draw() const {
	ImageButton::Draw();

}
