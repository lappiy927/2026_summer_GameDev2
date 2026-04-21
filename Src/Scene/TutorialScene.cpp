#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "TutorialScene.h"

TutorialScene::TutorialScene(void)
	:
	SceneBase()
{
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
{
}

void TutorialScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TutorialScene::Draw(void)
{
	DrawString(0, 0, "TutorialScene", 0xffffff);
}

void TutorialScene::Release(void)
{
}
