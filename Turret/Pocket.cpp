#include <allegro5/color.h>

#include "Pocket.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"

PlayScene* Pocket::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Pocket::Pocket(std::string img, std::string imgIn,  float x, float y, int money) :
    ImageButton(img, img, x, y), money(money), name(img), unname(imgIn) {
}
void Pocket::Update(float deltaTime) {
    ImageButton::Update(deltaTime);
    if (getPlayScene()->GetMoney() >= money) {
        Enabled = true;
        this->bmp = Engine::Resources::GetInstance().GetBitmap(name, Size.x, Size.y);
        Draw();
    } else {
        Enabled = false;
        this->bmp = Engine::Resources::GetInstance().GetBitmap(unname, Size.x, Size.y);
        Draw();
    }
}
void Pocket::Draw() const { ImageButton::Draw(); }