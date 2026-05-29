#include <DxLib.h>
#include <algorithm>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/BossStage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Weapon/Katana.h"
#include "BossScene.h"

BossScene::BossScene(void)
	:
	bossStage_(nullptr),
	SceneBase()
{
}

BossScene::~BossScene(void)
{
}

void BossScene::Init(void)
{
	// プレイヤー生成
	player_ = new Player();
	player_->Init();

	// ボスステージの初期化
	bossStage_ = new BossStage();
	bossStage_->Init();

	// 刀の初期化
	katana_ = new Katana(player_);
	katana_->Init();

	// ステージモデルのコライダーをプレイヤーに登録
	// ステージコライダー取得
	const ColliderBase* stageCollider =
		bossStage_->GetOwnCollider(static_cast<int>(BossStage::COLLIDER_TYPE::MODEL));

	// プレイヤーに登録
	player_->AddHitCollider(stageCollider);

	// カメラにプレイヤーを追従
	Camera* camera = sceMng_.GetCamera();
	camera->ChangeMode(Camera::MODE::FOLLOW);
	camera->SetFollow(&player_->GetTransform());
}

void BossScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	// プレイヤーの更新
	player_->Update();

	// ステージの更新
	bossStage_->Update();

	//刀の更新
	katana_->Update();

	// プレイヤー死亡
	if (player_->IsDead())
	{
		sceMng_.ChangeScene(
			SceneManager::SCENE_ID::GAMEOVER);

		return;
	}
}

void BossScene::Draw(void)
{
	// 描画
	bossStage_->Draw();

	player_->Draw();

	//刀の描画
	katana_->Draw();

	DrawString(0, 0, "BossScene", 0xffffff);

	VECTOR pPos = player_->GetPos();

	DrawFormatString(
		0, 100,
		0xffffff,
		"Player : %.2f %.2f %.2f",
		pPos.x, pPos.y, pPos.z);
}

void BossScene::Release(void)
{
	// プレイヤーの解放
	player_->Release();
	delete player_;

	// ステージの解放
	bossStage_->Release();
	delete bossStage_;

	//刀の開放
	katana_->Release();
	delete katana_;
}
