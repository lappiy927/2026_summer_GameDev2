#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Weapon/Katana.h"
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

	//刀の初期化
	katana_ = new Katana(player_);
	katana_->Init();

	// ステージモデルのコライダーをプレイヤーに登録
	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));
	player_->AddHitCollider(stageCollider);

	// カメラにプレイヤーを追従させる
	Camera* camera = sceMng_.GetCamera();
	camera->ChangeMode(Camera::MODE::FOLLOW);
	camera->SetFollow(&player_->GetTransform());
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

	//刀の更新
	katana_->Update();
}

void GameScene::Draw(void)
{
	DrawString(0, 0, "GameScene", 0xffffff);

	// 描画
	stage_->Draw();

	player_->Draw();

	//刀の描画
	katana_->Draw();
}

void GameScene::Release(void)
{
	// プレイヤーの解放
	player_->Release();
	delete player_;

	// ステージの解放
	stage_->Release();
	delete stage_;

	//刀の開放
	katana_->Release();
	delete katana_;
}
