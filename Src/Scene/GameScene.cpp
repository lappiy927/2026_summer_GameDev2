#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{

}

void GameScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
}

void GameScene::Draw(void)
{
	DrawString(0, 0, "GameScene", 0xffffff);
}

void GameScene::Release(void)
{
}
