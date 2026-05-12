#include "GameOverScene.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"
#include <DxLib.h>

GameOverScene::GameOverScene()
{
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Init()
{
}

void GameOverScene::Update()
{
	// Enter‚Åƒ^ƒCƒgƒ‹‚É–ß‚é
	if (CheckHitKey(KEY_INPUT_RETURN))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameOverScene::Draw()
{
	DrawString(
		500,
		300,
		"GAME OVER",
		GetColor(255, 0, 0));
}

void GameOverScene::Release()
{
}
