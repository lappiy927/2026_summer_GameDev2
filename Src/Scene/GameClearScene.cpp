#include "GameClearScene.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"
#include <DxLib.h>
#include "../Manager/InputManager.h"

GameClearScene::GameClearScene()
{
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Init()
{
	fontHandle =
		CreateFontToHandle(
			"Meiryo",
			128,
			6);
}

void GameClearScene::Update()
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

void GameClearScene::Draw()
{
	DrawStringToHandle(
		300,
		300,
		"GAME CLEAR",
		0xff0000,
		fontHandle);
}

void GameClearScene::Release()
{
	DeleteFontToHandle(fontHandle);
}
