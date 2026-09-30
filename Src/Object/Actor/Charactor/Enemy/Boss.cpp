#include "Boss.h"

#include <EffekseerForDXLib.h>
#include "../Player.h"

#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SoundManager.h"
#include "../../../../Application.h"

#include "../../../Common/AnimationController.h"

#include "../../../../Object/Collider/ColliderLine.h"
#include "../../../../Object/Collider/ColliderCapsule.h"

Boss::Boss()
	:
	EnemyBase(),
	attackCollider_(nullptr),
	isAttackHit_(false),
	attackEnable_(false),
	isAttacking_(false),
	leftHandFrame_(-1),
	chargeEffectHandle_(-1),
	chargeEffectPlaying_(false),
	chargeEffect_(-1)
{
	hp_ = BOSS_HP;

	attackRange_ = ATTACK_RANGE;
	searchRange_ = SEARCH_RANGE;
}

Boss::~Boss()
{
}

void Boss::InitLoad()
{
	// 共通ロード
	CharactorBase::InitLoad();

	// モデル読み込み
	int model =
		MV1DuplicateModel(
			resMng_.Load(
				ResourceManager::SRC::BOSS).handleId_);

	transform_.SetModel(model);

	effectHandle_ = LoadEffekseerEffect(_T("Data/Effect/blood.efk"), 50.0f);

	chargeEffect_ =LoadEffekseerEffect(_T("Data/Effect/Charge.efkefc"), 50.0f);
}

void Boss::InitTransform()
{
	transform_.scl_ = VGet(2.0f, 2.0f, 2.0f);

	transform_.quaRot_ = Quaternion::Identity();

	transform_.quaRotLocal_ = Quaternion::Identity();

	// 出現位置
	transform_.pos_ = VGet(2500.0f, 2000.0f, 6000.0f);

	transform_.Update();
}

void Boss::InitCollider()
{
	// 地面判定線
	ColliderLine* colLine =
		new ColliderLine(
			ColliderBase::TAG::BOSS,
			&transform_,
			VGet(0.0f, 100.0f, 0.0f),
			VGet(0.0f, -2000.0f, 0.0f));

	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::LINE),
		colLine);

	// 壁判定カプセル
	ColliderCapsule* colCapsule =
		new ColliderCapsule(
			ColliderBase::TAG::BOSS,
			&transform_,
			VGet(0.0f, 120.0f, 0.0f),
			VGet(0.0f, 0.0f, 0.0f),
			80.0f);

	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::CAPSULE),
		colCapsule);

	attackCollider_ =
		new ColliderCapsule(
			ColliderBase::TAG::BOSS,
			&transform_,
			VGet(0, 120, 80),
			VGet(0, 120, 250),
			60.0f);

	// 攻撃判定カプセル
	ownColliders_.emplace(
		static_cast<int>(COLLIDER_TYPE::ATTACK),
		attackCollider_);
}

void Boss::InitAnimation()
{
	animationController_ =
		new AnimationController(transform_.modelId_);

	// 待機
	animationController_->Add(
		0,
		20.0f,
		Application::PATH_MODEL + "Charactor/Enemy/Boss/BossIdle.mv1");

	// 歩き
	animationController_->Add(
		1,
		20.0f,
		Application::PATH_MODEL + "Charactor/Enemy/Boss/BossWalk.mv1");

	// ダッシュ
	animationController_->Add(
		2,
		20.0f,
		Application::PATH_MODEL + "Charactor/Enemy/Boss/BossRun.mv1");

	// 攻撃
	animationController_->Add(
		3,
		20.0f,
		Application::PATH_MODEL + "Charactor/Enemy/Boss/BossAttack.mv1");

	// チャージ
	animationController_->Add(
		4,
		20.0f,
		Application::PATH_MODEL + "Charactor/Enemy/Boss/Charge.mv1");

	// 死亡
	animationController_->Add(
		5,
		20.0f,
		Application::PATH_MODEL + "Charactor/Enemy/Boss/BossDai.mv1");

	// 初期アニメ
	animationController_->Play(0, true);
}

void Boss::InitPost()
{
	// 左手のフレーム番号取得
	leftHandFrame_ =
		MV1SearchFrame(
			transform_.modelId_,
			"hand.L"
		);
}

void Boss::AI()
{
	UpdateAttackCollider();

	if (isTackle_)
	{
		return;
	}

	// 行動中ならAIで状態を変えない
	if (state_ == STATE::DASH_READY ||
		state_ == STATE::DASH)
	{
		return;
	}

	if (target_ == nullptr)
	{
		return;
	}

	// 攻撃中なら他の行動をしない
	if (isAttacking_)
	{
		if (animationController_->IsEnd())
		{
			isAttacking_ = false;
			state_ = STATE::CHASE;

			animationController_->Play(1, true);
		}
		return;
	}

	float dist = GetPlayerDistance();

	// 死亡状態なら何もしない
	if (state_ == STATE::DEAD)
	{
		return;
	}

	// 攻撃距離内なら攻撃状態に遷移
	if (dist <= attackRange_)
	{
		state_ = STATE::ATTACK;

		isAttacking_ = true;
		animationController_->Play(3, false);
	}
	// 索敵距離内なら追跡状態に遷移	
	else if (dist <= 800.0f)
	{
		if (state_ != STATE::CHASE)
		{
			state_ = STATE::CHASE;
			animationController_->Play(1, true);
		}
	}
	// 索敵距離内ならチャージ状態に遷移
	else if (dist <= searchRange_)
	{
		if (state_ != STATE::DASH_READY)
		{
			state_ = STATE::DASH_READY;

			animationController_->Play(4, true);

			chargeEffectHandle_ =
				PlayEffekseer3DEffect(chargeEffect_);

			chargeEffectPlaying_ = true;
		}
		// チャージエフェクトの位置を更新
		if (chargeEffectPlaying_)
		{
			SetPosPlayingEffekseer3DEffect(
				chargeEffectHandle_,
				transform_.pos_.x,
				transform_.pos_.y + 120.0f,
				transform_.pos_.z);
		}
	}
	// 索敵距離外なら待機状態に遷移
	else
	{
		if (state_ != STATE::IDLE)
		{
			state_ = STATE::IDLE;
			animationController_->Play(0, true);
		}
	}

	attackEnable_ = (state_ == STATE::ATTACK);
}

void Boss::Damage(int damage)
{
	// 死亡状態ならダメージを受けない
	if (state_ == STATE::DEAD) return;

	hp_ -= damage;

	if (hp_ <= 0)
	{
		hp_ = 0;

		// 左肩のフレーム番号取得
		int shoulderFrame = MV1SearchFrame(transform_.modelId_, "shoulder.L");
		if (shoulderFrame != -1)
		{
			MATRIX shoulderMatrix = MV1GetFrameLocalWorldMatrix(transform_.modelId_, shoulderFrame);

			VECTOR rayOrigin = VGet(
				shoulderMatrix.m[3][0],
				shoulderMatrix.m[3][1],
				shoulderMatrix.m[3][2]);

			float yaw = transform_.quaRot_.ToEuler().y;
			VECTOR forward = VGet(sinf(yaw), 0.0f, cosf(yaw));

			effectPos_ = VAdd(rayOrigin, forward);

			// 死亡エフェクト再生
			int playHandle = PlayEffekseer3DEffect(effectHandle_);
			SetPosPlayingEffekseer3DEffect(playHandle, effectPos_.x, effectPos_.y, effectPos_.z);
			SetRotationPlayingEffekseer3DEffect(playHandle, 0.0f, yaw, 0.0f);
		}
		
		// チャージエフェクトを停止
		if (chargeEffectPlaying_)
		{
			StopEffekseer3DEffect(chargeEffectHandle_);
			chargeEffectPlaying_ = false;
			chargeEffectHandle_ = -1;
		}

		// 死亡音再生
		sndMng_.Play(SoundManager::SRC::EnemyDai);

		state_ = STATE::DEAD;

		animationController_->Play(5, false);
	}
}

bool Boss::IsAttack() const
{
	return attackEnable_;
}

void Boss::UpdateAttackCollider()
{
	if (leftHandFrame_ == -1 || attackCollider_ == nullptr)
	{
		return;
	}

	MATRIX mat =
		MV1GetFrameLocalWorldMatrix(
			transform_.modelId_,
			leftHandFrame_);

	VECTOR handPos =
	{
		mat.m[3][0],
		mat.m[3][1],
		mat.m[3][2]
	};

	VECTOR localPos = VSub(handPos, transform_.pos_);

	// 手のひら方向へ少しずらす
	localPos = VAdd(localPos, VGet(0.0f, 0.0f, 25.0f));


	attackCollider_->SetLocalPosTop(localPos);
	attackCollider_->SetLocalPosDown(localPos);
}
