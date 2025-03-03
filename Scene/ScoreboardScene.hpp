#ifndef ScoreboardScene_HPP
#define ScoreboardScene_HPP

#include <allegro5/allegro_audio.h>
#include <memory>
#include "Engine/IScene.hpp"
struct ScoreData {
    std::string name;
    int score;
};

bool compare(const ScoreData &a, const ScoreData & b);

class ScoreboardScene final : public Engine::IScene {
    
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int page;
    int pagemax;
public:
    explicit ScoreboardScene() : page(0){}

    void Initialize() override;

    void Terminate() override;

    void NextOnClick(int stage);

    void PrevOnClick(int stage);
    
    void BackOnClick(int stage);

    void setPage(int p)
    {
        page = p;
    }
    std::vector<ScoreData> LoadScoreData(const char *filename, std::vector<ScoreData> Scoreboard);
};








#endif // ScoreboardScene_HPP
