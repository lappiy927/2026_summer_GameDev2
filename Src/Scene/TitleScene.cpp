#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Application.h"
#include"../Object/Common/AnimationController.h"
#include"../Object/Common/Transform.h"
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

	imgTitle_ = LoadGraph("Data/Image/title.png");

	float size = 0;

	charactor_.SetModel(resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
	charactor_.pos = { 0.0f, -300.0f, -105.0f };
	size = 0.8f;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(
		0.0f, AsoUtility::Deg2RadF(90.0f), AsoUtility::Deg2RadF(45.0f));
	charactor_.Update();

	//アニメーションコントローラー
	animationController_ =
		new AnimationController(charactor_.modelId);
	animationController_->Add(0, 20.0f,
		Application::PATH_MODEL + "Charactor/Player/FastRun.mv1");
	animationController_->Play(0);

	enemy_.SetModel(resMng_.Load(ResourceManager::SRC::ENEMY).handleId_);
	enemy_.pos = { -100.0f, -300.0f, -105.0f };
	size = 0.4f;
	enemy_.scl = { size, size, size };
	enemy_.quaRot = Quaternion::Euler(
		AsoUtility::Deg2RadF(45.0f), 0.0f, AsoUtility::Deg2RadF(45.0f));
	enemy_.Update();
}

void TitleScene::Update(void)
{
	// キャラアニメーション
	animationController_->Update();

	// 惑星の回転
	enemy_.quaRot = enemy_.quaRot.Mult(
		Quaternion::Euler(AsoUtility::Deg2RadF(10.0f), AsoUtility::Deg2RadF(20.0f), AsoUtility::Deg2RadF(30.0f)));
	enemy_.Update();

	// シーン遷移
	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);


	if (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TUTORIAL);
	}
}

void TitleScene::Draw(void)
{
	DrawString(0, 0, "TitleScene", GetColor(255, 255, 255));
	DrawGraph(0, 0, imgTitle_, TRUE);
	MV1DrawModel(charactor_.modelId);
	MV1DrawModel(enemy_.modelId);
}

void TitleScene::Release(void)
{
	delete animationController_;
}