#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "ScoreboardScene.hpp"
#include "Engine/LOG.hpp"


bool compare(const ScoreData &a, const ScoreData & b)
{
    return a.score > b.score;
}

std::vector<ScoreData> ScoreboardScene::LoadScoreData(const char* filename, std::vector<ScoreData> Scoreboard)
{
    std::ifstream fin(filename);
    Scoreboard.clear();
    ScoreData data;
    while(fin >> data.name >> data.score)
    {
        std::cout << data.name << data.score;
        Scoreboard.push_back(data);
    }
    fin.close();
    std::sort(Scoreboard.begin(), Scoreboard.end(), compare);
    int t = Scoreboard.size()-1;
    while(t--) std::cout << Scoreboard[t].name << " " << Scoreboard[t].score << "\n";
    pagemax = (Scoreboard.size() / 8 +1);
    return Scoreboard;
}
void ScoreboardScene::Initialize() {
    std::cout << page << "openScoreboard" << "\n";
    std::cout << "!!!!" << pagemax << "\n";
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton* btn;
    Engine::ImageButton* btnnext;
    Engine::ImageButton* btnprev;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btnnext = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW*3/2-100, halfH * 7 / 4 - 50, 400, 100);
    btnprev = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW/2-300, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&ScoreboardScene::BackOnClick, this, 2));
    btnnext->SetOnClickCallback(std::bind(&ScoreboardScene::NextOnClick, this, 2));
    btnprev->SetOnClickCallback(std::bind(&ScoreboardScene::PrevOnClick, this, 2));
	AddNewControlObject(btn);
    AddNewControlObject(btnnext);
    AddNewControlObject(btnprev);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Next Page", "pirulen.ttf", 48, halfW*3/2+100, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Prev Page", "pirulen.ttf", 48, halfW/2-100, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("SCOREBOARD", "pirulen.ttf", 48, halfW, halfH * 1 / 4, 50, 205, 50, 255, 0.5, 0.5));
    std::vector<ScoreData> Scoreboard;
    Scoreboard = LoadScoreData("../Resource/scoreboard.txt", Scoreboard);
    for(int i=0; i<8; i++)
    {
        if(i+page*8 >= Scoreboard.size()) break;
        std::string data =  Scoreboard[i+page*5].name + " : " + std::to_string(Scoreboard[i+page*5].score); 
        AddNewObject(new Engine::Label(data, "pirulen.ttf", 35, halfW, halfH*2/5+50*i, 50, 205, 50, 255, 0.5, 0.5));
        //AddNewObject(new Engine::Label(data, "pirulen.ttf", 24, halfW-200, 40*i, 255, 255, 255, 0, 0.5, 0.5));
    }
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}

void ScoreboardScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void ScoreboardScene::BackOnClick(int stage) {
// Change to select scene.
Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void ScoreboardScene::PrevOnClick(int stage) {
// Change to select scene.
if(page>0)page--;
Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void ScoreboardScene::NextOnClick(int stage) {
// Change to select scene.
if(page<pagemax-1)page++;
Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

