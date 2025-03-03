    #ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <ctime>
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Turret/Pocket.hpp"


class Turret;
class Character;
class Tower;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}  // namespace Engine

class PlayScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
protected:
    int lives;
    int money;
    int SpeedMult;
public:
    int MoneyLimit;
    static bool DebugMode;
    static const std::vector<Engine::Point> directions;
	static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    static const std::vector<int> code;
    int MapId;
    bool shoot;
    float ticks;
    float explosionx;
    int count;
    Pocket* pocket;
    time_t preshoot; //need to assign to now after the shootbtn click
    time_t now;
    //tm *time = localtime(&now);
    float deathCountDown;
    // Map tiles.
    Group* TileMapGroup;
    Group* GroundEffectGroup;
    Group* DebugIndicatorGroup;
    Group* BulletGroup;
    Group* BadtowerGroup;
    Group* CharacterGroup;
    Group* EnemyGroup;
    Group* EffectGroup;
    Group* UIGroup;
    Group* Explosion;
    Group* CatowerGroup;
    Engine::Label* UIMoney;
    Engine::Label* UILives;
    Engine::Label* CatowerHP;
    Engine::Label* EnemytowerHP;
    Engine::Image* imgTarget;
    //std::vector(Engine::Image*, float explodex);
    Engine::Sprite* dangerIndicator;
    Turret* preview;
    Tower* catt, * badd;
    Character* cha;
    //int CatOgary;
    bool NewCharacter;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::vector<std::vector<int>> revmapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
	void Hit();
    int GetMoney() const;
    void EarnMoney(int money);
    int GetTime() const;
    int Getcount() const;
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    void CatBtnClicked(int id);
    void ShootButtonClicked(int id);
    //bool CheckSpaceValid(int x, int y);
    void SaveScore(const std::string filename);

    std::vector<std::vector<int>> CalculateBFSDistance();
    // void ModifyReadMapTiles();
};
#endif // PLAYSCENE_HPP