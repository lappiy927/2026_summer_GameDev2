#include "GameOverScene.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"
#include <DxLib.h>
#include "../Manager/InputManager.h"

GameOverScene::GameOverScene()
{
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Init()
{
	fontHandle =
		CreateFontToHandle(
			"Meiryo",
			128,
			6);
}

void GameOverScene::Update()
{
	// ƒ^ƒCƒgƒ‹‚É–ß‚é
	auto const& ins = InputManager::GetInstance();
	
	if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
		ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::DOWN))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameOverScene::Draw()
{
	DrawStringToHandle(
		300,
		300,
		"GAME OVER",
		0xff0000,
		fontHandle);
}

void GameOverScene::Release()
{
	DeleteFontToHandle(fontHandle);
}
