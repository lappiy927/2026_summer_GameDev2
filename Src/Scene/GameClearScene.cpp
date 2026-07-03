#include "GameClearScene.h"
#include "../Manager/SceneManager.h"
#include "TitleScene.h"
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/ResourceManager.h" 
#include "../Manager/Camera.h"
#include "../Application.h"
#include <algorithm>
#include "../Object/GameClearUI.h"

GameClearScene::GameClearScene()
{
}

GameClearScene::~GameClearScene()
{
}

void GameClearScene::Init()
{
	// 定点カメラ
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	float size = 0;

	charactor_.SetModel(resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
	charactor_.pos = { 0.0f, -200.0f, -105.0f };
	size = 2.5f;
	charactor_.scl = { size, size, size };
	charactor_.quaRot = Quaternion::Euler(
		0.0f, AsoUtility::Deg2RadF(90.0f), AsoUtility::Deg2RadF(45.0f));
	charactor_.Update();

	// アニメーションコントローラー
	animationController_ =
		new AnimationController(charactor_.modelId);
	animationController_->Add(0, 20.0f,
		Application::PATH_MODEL + "Charactor/Player/FastRun.mv1");
	animationController_->Play(0);

	// フォント
	fontHandle =
		CreateFontToHandle(
			"Meiryo",
			128,
			6);

	// テキストアニメ初期化
	textAnimTimer_ = 0.0f;
	currentTextY_ = textEndY_;
	currentTextAlpha_ = 0;
	showText_ = false;

	// GameClearUI 初期化 ▼追加
	gameClearUI_.screenW = 1280;
	gameClearUI_.screenH = 720;
	gameClearUI_.Init();
}

void GameClearScene::Update()
{
	// タイトルに戻る
	auto const& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_RETURN) ||
		ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::RIGHT))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	// キャラアニメーション
	if (animationController_ != nullptr)
	{
		animationController_->Update();
	}

	// テキスト表示開始は遅延後
	if (!showText_)
	{
		timer_ += sceMng_.GetDeltaTime();
		if (timer_ >= textDelay_)
		{
			showText_ = true;
			textAnimTimer_ = 0.0f;
		}
	}

	// テキスト奥→手前アニメーション
	if (showText_)
	{
		textAnimTimer_ += sceMng_.GetDeltaTime();
		float t = textAnimTimer_ / textAnimDuration_;
		if (t > 1.0f) t = 1.0f;

		currentTextY_ = static_cast<int>(
			textStartY_ + (textEndY_ - textStartY_) * t);

		currentTextAlpha_ = static_cast<int>(255.0f * t);
		currentTextAlpha_ = std::clamp(currentTextAlpha_, 0, 255);
	}

	// GameClearUI 更新 ▼追加
	gameClearUI_.Update();
}

void GameClearScene::Draw()
{
	// 3Dモデルを描画
	if (charactor_.modelId != -1)
	{
		MV1DrawModel(charactor_.modelId);
	}

	// GAME OVER 表示（奥から手前に近づく演出）
	if (showText_)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, currentTextAlpha_);
		DrawStringToHandle(
			300,
			currentTextY_,
			"死合 終了",
			0xff0000,
			fontHandle);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// GameClearUI 描画 ▼追加
	gameClearUI_.Draw();
}

void GameClearScene::Release()
{
	DeleteFontToHandle(fontHandle);

	if (animationController_ != nullptr)
	{
		delete animationController_;
		animationController_ = nullptr;
	}

	charactor_.Release();

	// GameClearUI 解放 ▼追加
	gameClearUI_.Release();
}

