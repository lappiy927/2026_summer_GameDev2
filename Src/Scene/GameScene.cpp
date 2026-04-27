#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	stage_(nullptr),
	SceneBase()
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// プレイヤー生成
	player_ = new Player();
	player_->Init();

	// ステージの初期化
	stage_ = new Stage();
	stage_->Init();

	// カメラにプレイヤーを追従させる
	Camera* camera = sceMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());
	camera->ChangeMode(Camera::MODE::FOLLOW);
}

void GameScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	// プレイヤーの更新
	player_->Update();

	// ステージの更新
	stage_->Update();
}

void GameScene::Draw(void)
{
	DrawString(0, 0, "GameScene", 0xffffff);

	// 描画
	stage_->Draw();

	player_->Draw();


}

void GameScene::Release(void)
{
	// プレイヤーの解放
	player_->Release();
	delete player_;

	// ステージの解放
	stage_->Release();
	delete stage_;
}
