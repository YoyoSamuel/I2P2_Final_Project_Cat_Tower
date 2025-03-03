#include <functional>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <iostream>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "PlayScene.hpp"
#include "Engine/Point.hpp"
#include "WinScene.hpp"

/*void WinScene::SaveName(const std::string filename, std::string name)
{
 	std::ofstream file;
	file.open(filename, std::ios::app);
	if(!file.is_open())
		std::cout<<"wrong";
	file << name << " " << "\n";
	std::cout << name <<"save success!!\n";
	file.close();
}*/

void WinScene::Initialize() {
	ticks = 0;
	int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
	int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
	int halfW = w / 2;
	int halfH = h / 2;
	AddNewObject(new Engine::Image("win/fortype.png", halfW, halfH/3 + 10, 0, 0, 0.5, 0.5));
	AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 -10, 255, 255, 255, 255, 0.5, 0.5));
	Engine::ImageButton* btn;
	btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
	btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
	AddNewControlObject(btn);
	AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
	bgmId = AudioHelper::PlayAudio("win.wav");
	//SaveName("../Resource/scoreboard.txt", name);
}
void WinScene::Terminate() {
	IScene::Terminate();
	AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
	ticks += deltaTime;
	if (ticks > 4 && ticks < 100 &&
		dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
		ticks = 100;
		bgmId = AudioHelper::PlayBGM("happy.ogg");
	}
}
void WinScene::BackOnClick(int stage) {
	// Change to select scene.
	Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
/*
std::string WinScene::getUserName()
{
	
	ALLEGRO_EVENT_QUEUE* eventQueue = al_create_event_queue();
	ALLEGRO_EVENT event;
	std::string input;
	bool EnterPressed = false;
	while(!EnterPressed)
	{
		
		al_wait_for_event(eventQueue, &event);
		if(event.type == ALLEGRO_EVENT_KEY_CHAR)
		{
			if (event.keyboard.unichar >= 32 && event.keyboard.unichar <= 126) {
                // Append the character to the input string if it's a printable character
                input += static_cast<char>(event.keyboard.unichar);
            } else if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && !input.empty()) {
                // Remove the last character if backspace is pressed
                input.pop_back();
            } else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                // Finish input if Enter is pressed
                EnterPressed = true;
            }
		}
		AddNewObject(new Engine::Label(input, "pirulen.ttf", 48, 20, 40, 255, 255, 255, 255, 0.5, 0.5));
	} 
	return input;
}

void WinScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_EVENT_KEY_CHAR) {
		DebugMode = !DebugMode;
	}
	else {
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size())
			keyStrokes.pop_front();
		if (keyCode == ALLEGRO_KEY_ENTER && keyStrokes.size() == code.size()) {
			auto it = keyStrokes.begin();
			for (int c : code) {
				if (!((*it == c) ||
					(c == ALLEGRO_KEYMOD_SHIFT &&
					(*it == ALLEGRO_KEY_LSHIFT || *it == ALLEGRO_KEY_RSHIFT))))
					return;
				++it;
			}
			EffectGroup->AddNewObject(new Plane());
			EarnMoney(10000);
		}
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for MachineGunTurret.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		// Hotkey for LaserTurret.
		UIBtnClicked(1);
	}
	else if (keyCode == ALLEGRO_KEY_E) {
		// Hotkey for MissileTurret.
		UIBtnClicked(2);
	}
	// TODO: [CUSTOM-TURRET]: Make specific key to create the turret.
	else if (keyCode == ALLEGRO_KEY_R) {
		// Hotkey for ThreeBulletTurret.
		UIBtnClicked(3);
	}
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}
*/
