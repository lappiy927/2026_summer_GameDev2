#include "GameClearScene.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"
#include <DxLib.h>

GameClearScene::GameClearScene()
{
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Init()
{
}

void GameClearScene::Update()
{
	// Enter‚Åƒ^ƒCƒgƒ‹‚É–ß‚é
	if (CheckHitKey(KEY_INPUT_RETURN))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameClearScene::Draw()
{
	DrawString(
		500,
		300,
		"GAME CLEAR",
		GetColor(255, 0, 0));
}

void GameClearScene::Release()
{
}
