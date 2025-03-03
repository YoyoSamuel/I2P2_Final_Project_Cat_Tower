#ifndef TOWERSHOOTBUTTON_HPP
#define TOWERSHOOTBUTTON_HPP
#include <string>

#include "UI/Component/ImageButton.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class TowerButton : public Engine::ImageButton {
protected:
    PlayScene* getPlayScene();
public:
	int CoolTime;
	TowerButton(std::string img, std::string imgIn, float x, float y, int CoolTime);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // TOWERSHOOTBUTTON_HPP
