#ifndef POCKET_HPP
#define POCKET_HPP
#include <string>

#include "UI/Component/ImageButton.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class Pocket : public Engine::ImageButton {
protected:
    PlayScene* getPlayScene();
public:
    int money;
    std::string name, unname;
    Pocket(std::string img, std::string imgIn, float x, float y, int money);
    void Update(float deltaTime) override;
    void Draw() const override;
};
#endif // POCKET_HPP