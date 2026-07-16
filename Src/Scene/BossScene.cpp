#include <DxLib.h>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include"../Application.h"
#include "../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include"../Manager/WeaponManager.h"
#include "../Object/Actor/Stage/BossStage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/Boss.h"
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

	// 武器の初期化
	weaponMng_ = new WeaponManager();
	weaponMng_->Init();
	player_->SetWeaponManager(weaponMng_);

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

	//武器の更新
	weaponMng_->Update(
		player_->GetTransform(),
		player_->GetWeaponState());

	if (!boss_->IsDead() && boss_->IsAttack())
	{
		ColliderCapsule* attack = boss_->GetAttackCollider();
		ColliderCapsule* playerCol =
			dynamic_cast<ColliderCapsule*>(
				player_->GetCollider(
					static_cast<int>(Player::COLLIDER_TYPE::CAPSULE)));

		if (attack && playerCol && attack->IsHit(playerCol))
		{
			player_->Damage(20);
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
				weaponMng_->GetActiveCollider()->IsHit(enemyCol);

			DrawFormatString(
				0, 140,
				0xff0000,
				"SwordHit : %d",
				hit);

			if (hit)
			{
				weaponMng_->SetCollider(false);
				if (weaponMng_->GetActiveWeaponType() == WeaponManager::WEAPON_TYPE::KATANA) {
					boss_->Damage(100);
				}
				else {
					boss_->Damage(40);
					int effect = PlayEffekseer3DEffect(slashEffectHandle_);

					VECTOR pos = boss_->GetPos();

					SetPosPlayingEffekseer3DEffect(
						effect,
						pos.x,
						pos.y + 50.0f,
						pos.z);
				}
				

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

	//武器の描画
	weaponMng_->Draw();

	DrawString(0, 0, "BossScene", 0xffffff);

	VECTOR pPos = player_->GetPos();

	/*DrawFormatString(
		0, 100,
		0xffffff,
		"Player : %.2f %.2f %.2f",
		pPos.x, pPos.y, pPos.z);*/

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

	//武器の開放
	weaponMng_->Release();
	delete weaponMng_;
}
