#include <DxLib.h>
#include <algorithm>
#include <EffekseerForDXLib.h>
#include"../Application.h"
#include "../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include"../Manager/WeaponManager.h"	
#include "../Scene/MenuScene.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Stage/Door.h"
#include"../Object/Actor/Stage/Grass.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Weapon/Gun.h"
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
	// 残弾数をリセット
	Gun::ResetRemainingBullets();

	// プレイヤー生成
	player_ = new Player();
	player_->Init();

	// ステージの初期化
	stage_ = new Stage();
	stage_->Init();

	//武器の初期化
	weaponMng_ = new WeaponManager();
	weaponMng_->Init();
	player_->SetWeaponManager(weaponMng_);

	door_ = new Door();
	door_->Init();

	grass_ = std::make_unique<Grass>();
	grass_->Init();

	grass_->AddHitCollider(stage_->GetModelCollider());
	// ここで散布
	grass_->GenerateField(10000, 10000.0f);

	// ステージモデルのコライダーをプレイヤーに登録
	// ステージコライダー取得
	const ColliderBase* stageCollider =
		stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));

	// プレイヤーに登録
	player_->AddHitCollider(stageCollider);

	for (int i = 0; i < 10; i++)
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

	remainEnemy_ = static_cast<int>(enemies_.size());

	// カメラにプレイヤーを追従
	Camera* camera = sceMng_.GetCamera();
	camera->SetFollow(&player_->GetTransform());
	camera->ChangeMode(Camera::MODE::FOLLOW);

	sndMng_.Play(SoundManager::SRC::Battle, true, 200);

	limitTime_ = 180.0f;



	lifeHandle_ = LoadGraph("Data/Image/Life.png");

	daiHandle_ = LoadGraph("Data/Image/Dai.png");

	timeUI_ = LoadGraph("Data/Image/Time_UI.png");

	slashEffectHandle_ =
		LoadEffekseerEffect(
			"Data/Effect/Slash.efkefc",20.0f);

	countImage_[0] = LoadGraph("Data/Image/San.png");
	countImage_[1] = LoadGraph("Data/Image/Ni.png");
	countImage_[2] = LoadGraph("Data/Image/Ichi.png");
	countImage_[3] = LoadGraph("Data/Image/Hajime.png");


}

void GameScene::Update(void)
{
	if (bossRoomOpenTimer_ > 0)
	{
		bossRoomOpenTimer_--;
	}

	if (!isGameStart_)
	{
		countDown_ -= sceMng_.GetDeltaTime();

		if (countDown_ <= 0.0f)
		{
			isGameStart_ = true;
		}

		return;
	}



	UpdateEffekseer3D();

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

	// 落下判定
	if (player_->GetPos().y < -200.0f)
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAMEOVER);
		return;
	}


	// ステージの更新
	stage_->Update();

	//刀の更新
	weaponMng_->Update(
		player_->GetTransform(),
		player_->GetWeaponState());
	hit_ = false;

	door_->Update();

	grass_->SetPlayerPos(player_->GetPos());
	grass_->Update();

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
				weaponMng_->GetActiveCollider()->IsHit(enemyCol);

			DrawFormatString(
				0, 140,
				0xff0000,
				"SwordHit : %d",
				hit);

			if (hit)
			{
				// ダメージ前のHP
				int beforeHp = enemy->GetHp();

				enemy->Damage(100);

				if (beforeHp > 0 && enemy->GetHp() <= 0)
				{
					remainEnemy_--;

					if (remainEnemy_ > 0)
					{
						remainEnemyTimer_ = 120;
					}

					if (remainEnemy_ == 0 && !isBossRoomOpen_)
					{
						isBossRoomOpen_ = true;
						door_->Open();
						bossRoomOpenTimer_ = 180;
					}
				}

				if (weaponMng_->GetActiveWeaponType() == WeaponManager::WEAPON_TYPE::KATANA)
				{
					int effect = PlayEffekseer3DEffect(slashEffectHandle_);

					VECTOR pos = enemy->GetPos();

					SetPosPlayingEffekseer3DEffect(
						effect,
						pos.x,
						pos.y + 50.0f,
						pos.z);
				}
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

	int before = enemies_.size();

	enemies_.erase(
		std::remove_if(
			enemies_.begin(),
			enemies_.end(),
			[](const std::shared_ptr<EnemyBase>& enemy)
			{
				return enemy->IsDead();
			}),
		enemies_.end());

	if (remainEnemyTimer_ > 0)
	{
		remainEnemyTimer_--;
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
	grass_->Draw();

	player_->Draw();

	//武器の描画
	weaponMng_->Draw();

	door_->Draw();


	int y = 40;

	VECTOR pPos = player_->GetPos();

	//DrawFormatString(
	//	0, 100,
	//	0xffffff,
	//	"Player : %.2f %.2f %.2f",
	//	pPos.x, pPos.y, pPos.z);

	for (auto& enemy : enemies_)
	{
		enemy->Draw();

		VECTOR pos = enemy->GetPos();

	/*	DrawFormatString(
			0, y,
			0xffffff,
			"Enemy : %.2f %.2f %.2f",
			pos.x, pos.y, pos.z);*/

		y += 20;

		/*DrawFormatString(0, 80, 0xffffff,
			"HitCheck: %d",
			enemy->IsHit(player_) ? 1 : 0);*/
	}

	//DrawFormatString(
	//	0, 20,
	//	0x000000,
	//	"TIME : %.1f",
	//	limitTime_);

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

		// フォントサイズ64
		int font = CreateFontToHandle(
			NULL,
			64,
			3,
			DX_FONTTYPE_ANTIALIASING_EDGE);

		const char* text = "ボス部屋が　開いた！";

		// 文字サイズ取得
		int textWidth = GetDrawStringWidthToHandle(
			text,
			strlen(text),
			font);

		int textHeight = GetFontSizeToHandle(font);

		// 画面中央
		int x = (1280 - textWidth) / 2;
		int y = (720 - textHeight) / 2;

		if (bossRoomOpenTimer_ > 0)
		{
			DrawStringToHandle(
				x,
				y,
				text,
				GetColor(255, 0, 0),
				font);
		}
		

		DeleteFontToHandle(font);
	}

	if (remainEnemyTimer_ > 0)
	{
		int font = CreateFontToHandle(
			NULL,
			64,    // フォントサイズ
			3,
			DX_FONTTYPE_ANTIALIASING_EDGE);

		char text[32];
		sprintf_s(text, "あと %d体", remainEnemy_);

		// 文字サイズ取得
		int textWidth = GetDrawStringWidthToHandle(
			text,
			strlen(text),
			font);

		int textHeight = GetFontSizeToHandle(font);

		// 画面中央
		int x = (Application::SCREEN_SIZE_X - textWidth) / 2;
		int y = (Application::SCREEN_SIZE_Y - textHeight) / 2;

		DrawStringToHandle(
			x,
			y,
			text,
			GetColor(255, 255, 0),
			font);

		DeleteFontToHandle(font);
	}



	// TIMEUI
	DrawGraph(-20, -20, timeUI_, TRUE);

	int minute = static_cast<int>(limitTime_) / 60;
	int second = static_cast<int>(limitTime_) % 60;

	// TIME
	DrawFormatString(
		50, 
		60,
		GetColor(0, 0, 0),
		"%02d:%02d",
		minute,
		second);

	DrawFormatString(45, 40, GetColor(0, 0, 0), "残り時間");


	// HP
	if (!player_->IsDead())
	{
		DrawGraph(200, 25, lifeHandle_, TRUE);
	}
	else
	{
		DrawGraph(200, 25, daiHandle_, TRUE);
	}


	if (!isGameStart_)
	{
		int handle = -1;

		if (countDown_ > 3.0f)
		{
			handle = countImage_[0];
		}
		else if (countDown_ > 2.0f)
		{
			handle = countImage_[1];
		}
		else if (countDown_ > 1.0f)
		{
			handle = countImage_[2];
		}
		else if (countDown_ > 0.0f)
		{
			handle = countImage_[3];
		}

		if (handle != -1)
		{
			DrawGraph(
				Application::SCREEN_SIZE_X / 2 - 256,
				Application::SCREEN_SIZE_Y / 2 - 256,
				handle,
				TRUE);
		}
	}

	Effekseer_Sync3DSetting();

	DrawEffekseer3D();
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
	weaponMng_->Release();
	delete weaponMng_;

	// ドアの解放
	door_->Release();
	delete door_;

	grass_->Release();
	
}
