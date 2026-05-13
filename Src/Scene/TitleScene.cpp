#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Application.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	// 定点カメラ
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
	}

}

void TitleScene::Draw(void)
{
	DrawString(0, 0, "TitleScene", GetColor(255,255,255));

}

void TitleScene::Release(void)
{

}