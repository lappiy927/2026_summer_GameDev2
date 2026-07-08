#include <DxLib.h>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include"../Application.h"
#include "../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/BossStage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/Boss.h"
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

	// ボス生成
	boss_ = new Boss();
	boss_->Init();
	boss_->SetTarget(player_);

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

	// ボスに登録
	boss_->AddHitCollider(stageCollider);

	// カメラにプレイヤーを追従
	Camera* camera = sceMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());

	sndMng_.Play(SoundManager::SRC::Battle);

	slashEffectHandle_ =
		LoadEffekseerEffect(
			"Data/Effect/Slash.efkefc", 20.0f);
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

	// ボスの更新
	boss_->Update();

	// ステージの更新
	bossStage_->Update();

	//刀の更新
	katana_->Update();

	// 敵がプレイヤーに当たった
	if (!boss_->IsDead())
	{
		ColliderCapsule* attack = boss_->GetAttackCollider();

		if (attack != nullptr)
		{
			ColliderCapsule* playerCol =
				dynamic_cast<ColliderCapsule*>(
					player_->GetCollider(
						static_cast<int>(
							Player::COLLIDER_TYPE::CAPSULE)));

			if (playerCol != nullptr)
			{
				if (attack->IsHit(playerCol))
				{
					player_->Damage(999);
				}
			}
		}
	}

	if (!boss_->IsDead())
	{
		ColliderCapsule* enemyCol =
			dynamic_cast<ColliderCapsule*>(
				boss_->GetCollider(
					static_cast<int>(
						EnemyBase::COLLIDER_TYPE::CAPSULE)));

		if (enemyCol != nullptr)
		{
			bool hit =
				katana_->GetCollider()->IsHit(enemyCol);

			DrawFormatString(
				0, 140,
				0xff0000,
				"SwordHit : %d",
				hit);

			if (hit)
			{
				boss_->Damage(50);

				int effect = PlayEffekseer3DEffect(slashEffectHandle_);

				VECTOR pos = boss_->GetPos();

				SetPosPlayingEffekseer3DEffect(
					effect,
					pos.x,
					pos.y + 50.0f,
					pos.z);
			}
		}
	}
	

	if (boss_->IsDeadAnimationEnd())
	{
		sndMng_.StopAll();
		sceMng_.ChangeScene(
			SceneManager::SCENE_ID::GAMECLEAR);

		return;
	}
	else if (player_->IsDead())
	{
		sndMng_.StopAll();
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

	boss_->Draw();

	//刀の描画
	katana_->Draw();

	DrawString(0, 0, "BossScene", 0xffffff);

	VECTOR pPos = player_->GetPos();

	DrawFormatString(
		0, 100,
		0xffffff,
		"Player : %.2f %.2f %.2f",
		pPos.x, pPos.y, pPos.z);

	if (hit_) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // 半透明(0～255)
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
			GetColor(255, 0, 0),
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	Effekseer_Sync3DSetting();

	DrawEffekseer3D();
}

void BossScene::Release(void)
{
	// プレイヤーの解放
	player_->Release();
	delete player_;

	// ボスの解放
	boss_->Release();
	delete boss_;

	// ステージの解放
	bossStage_->Release();
	delete bossStage_;

	//刀の開放
	katana_->Release();
	delete katana_;
}
