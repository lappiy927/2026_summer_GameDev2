#include <DxLib.h>
#include <algorithm>
#include"../Application.h"
#include "../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Scene/MenuScene.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/Door.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Weapon/Katana.h"
#include "../Object/Actor/Charactor/Enemy/EnemyMob.h"
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

	door_ = new Door();
	door_->Init();

	// ステージモデルのコライダーをプレイヤーに登録
	// ステージコライダー取得
	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	// プレイヤーに登録
	player_->AddHitCollider(stageCollider);

	for (int i = 0; i < 1; i++)
	{
		auto enemy = std::make_shared<EnemyMob>();

		enemy->Init();

		enemy->SetTarget(player_);

		enemy->AddHitCollider(stageCollider);

		VECTOR pos = VGet(0, 50, 0);

		bool found = false;

		for (int attempt = 0; attempt < 100; attempt++)
		{
			float x =
				(rand() % 7000) - 3500.0f;

			float z =
				(rand() % 7000) - 3500.0f;

			// 山中心からの距離
			float centerDist = sqrtf(x * x + z * z);

			// 山の内側ならやり直し
			if (centerDist < 600.0f)
			{
				continue;
			}

			pos = VGet(x, 2000.0f, z);

			bool isNear = false;

			for (auto& other : enemies_)
			{
				float dist =
					VSize(
						VSub(
							other->GetPos(),
							pos));

				if (dist < 200.0f)
				{
					isNear = true;
					break;
				}
			}

			if (!isNear)
			{
				found = true;
				break;
			}
		}

		// 有効位置が見つかった時だけ生成
		if (found)
		{
			enemy->SetPos(pos);

			enemies_.push_back(enemy);
		}
	}

	// カメラにプレイヤーを追従
	Camera* camera = sceMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());
	camera->ChangeMode(Camera::MODE::FOLLOW);

	sndMng_.Play(SoundManager::SRC::Battle, true, 200);

	limitTime_ = 500.0f;
}

void GameScene::Update(void)
{


	limitTime_ -= sceMng_.GetDeltaTime();

	if (limitTime_ <= 0.0f)
	{
		limitTime_ = 0.0f;

		sceMng_.ChangeScene(
			SceneManager::SCENE_ID::TITLE);
	}

	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	

	if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
		ins.IsPadBtnTrgDown(
		InputManager::JOYPAD_NO::PAD1,
		InputManager::JOYPAD_BTN::MENU))
	{
		sndMng_.StopAll();
		sceMng_.PushScene(new MenuScene());
	}

	// プレイヤーの更新
	player_->Update();

	// ステージの更新
	stage_->Update();

	//刀の更新
	katana_->Update();
	hit_ = false;

	door_->Update();

	// エネミーの更新
	for (auto& enemy : enemies_)
	{
		enemy->Update();

		// 敵がプレイヤーに当たった
		if (enemy->IsHit(player_))
		{
			hit_ = true;

			player_->Damage(999);
		}

		ColliderCapsule* enemyCol =
			dynamic_cast<ColliderCapsule*>(
				enemy->GetCollider(
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
				enemy->Damage(50);

			}
		};
	}

	// プレイヤー死亡
	if (player_->IsDead())
	{
		sndMng_.StopAll();
		sceMng_.ChangeScene(
			SceneManager::SCENE_ID::GAMEOVER);

		return;
	}

	enemies_.erase(
		std::remove_if(
			enemies_.begin(),
			enemies_.end(),
			[](const std::shared_ptr<EnemyBase>& enemy)
			{
				return enemy->IsDead();
			}),
		enemies_.end());

	if (enemies_.empty() && !isBossRoomOpen_)
	{
		isBossRoomOpen_ = true;

		door_->Open();
	}

	if (isBossRoomOpen_)
	{
		VECTOR playerPos = player_->GetPos();

		// ドア中央位置
		VECTOR goalPos = VGet(200.0f, 400.0f, 100.0f);

		float dist =
		VSize(
			VSub(playerPos, goalPos));

		//　ドアに近づいた
		if (dist < 875.0f)
		{
			sndMng_.StopAll();
			sceMng_.ChangeScene(
				SceneManager::SCENE_ID::BOSS);
		}
	}
}

void GameScene::Draw(void)
{

	// 描画
	stage_->Draw();

	player_->Draw();

	//刀の描画
	katana_->Draw();

	door_->Draw();

	int y = 40;

	VECTOR pPos = player_->GetPos();

	DrawFormatString(
		0, 100,
		0xffffff,
		"Player : %.2f %.2f %.2f",
		pPos.x, pPos.y, pPos.z);

	for (auto& enemy : enemies_)
	{
		enemy->Draw();

		VECTOR pos = enemy->GetPos();

		DrawFormatString(
			0, y,
			0xffffff,
			"Enemy : %.2f %.2f %.2f",
			pos.x, pos.y, pos.z);

		y += 20;

		DrawFormatString(0, 80, 0xffffff,
			"HitCheck: %d",
			enemy->IsHit(player_) ? 1 : 0);
	}

	DrawFormatString(
		0, 20,
		0x000000,
		"TIME : %.1f",
		limitTime_);

	DrawString(0, 0, "GameScene", 0xffffff);

	if (hit_)
	{
		DrawString(0, 100, "HIT!", 0xff0000);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128); // 半透明(0～255)
		DrawBox(
			0, 0,
			Application::SCREEN_SIZE_X, Application::SCREEN_SIZE_Y,
			GetColor(255, 0, 0),
			TRUE
		);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	if (isBossRoomOpen_)
	{
		DrawString(
			500,
			0,
			"BOSS ROOM OPEN",
			0xff0000);
	}

	DrawFormatString(
		0, 600, 0xffffff,
		"ESC=%d",
		CheckHitKey(KEY_INPUT_ESCAPE));
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

	// ドアの解放
	door_->Release();
	delete door_;
}
