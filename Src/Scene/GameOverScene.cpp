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
	fontHandle =
		CreateFontToHandle(
			"Meiryo",
			128,
			6);

	DrawStringToHandle(
		300,
		300,
		"GAME OVER",
		0xff0000,
		fontHandle);
}

void GameOverScene::Release()
{
}
