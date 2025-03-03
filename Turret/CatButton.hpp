#ifndef CATBUTTON_HPP
#define CATBUTTON_HPP
#include <string>

#include "UI/Component/ImageButton.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class CatButton : public Engine::ImageButton {
protected:
    PlayScene* getPlayScene();
public:
	int money;
	std::string name, unname;
	CatButton(std::string img, std::string imgIn, float x, float y, int money);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // CATTBUTTON_HPP
