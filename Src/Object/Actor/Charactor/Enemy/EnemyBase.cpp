#include "EnemyBase.h"
#include <EffekseerForDXLib.h>
#include "../Player.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../Common/AnimationController.h"
#include "../../../../Object/Collider/ColliderCapsule.h"

EnemyBase::EnemyBase()
	:
	CharactorBase(),
	state_(STATE::IDLE),
	target_(nullptr),
	hp_(100),
	isDead_(false),
	searchRange_(700.0f),
	attackRange_(50.0f),
	moveSpeedEnemy_(5.0f)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::UpdateProcess()
{
	if (!isLanding_)
	{
		// 地面につくまでAI停止
		if (!isJump_)
		{
			isLanding_ = true;
		}
		else
		{
			transform_.Update();
			return;
		}
	}

	if (state_ != STATE::DASH)
	{
		//AI
		AI();
	}

	// 状態更新
	switch (state_)
	{
	case STATE::IDLE:
		UpdateIdle();
		break;

	case STATE::CHASE:
		UpdateChase();
		break;

	case STATE::DASH_READY:
		UpdateDashReady();
		break;

	case STATE::DASH:
		UpdateDash();
		break;

	case STATE::ATTACK:
		UpdateAttack();
		break;

	case STATE::DAMAGE:
		UpdateDamage();
		break;

	case STATE::DEAD:
		UpdateDead();
		break;
	}

	transform_.Update();
}

void EnemyBase::UpdateProcessPost()
{
}

void EnemyBase::SetTarget(Player* player)
{
	target_ = player;
}

bool EnemyBase::IsDead() const
{
	return isDead_;
}

bool EnemyBase::IsDeadAnimationEnd() const
{
	if (state_ != STATE::DEAD)
	{
		return false;
	}

	if (animationController_ == nullptr)
	{
		return true;
	}

	return animationController_->IsEnd();
}

void EnemyBase::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

bool EnemyBase::IsHit(Player* player)
{
	// 死亡演出中は当たり判定を無効化
	if (state_ == STATE::DEAD) return false;

	auto it = ownColliders_.find((int)COLLIDER_TYPE::CAPSULE);
	if (it == ownColliders_.end()) return false;

	auto myCol = dynamic_cast<ColliderCapsule*>(it->second);
	if (!myCol) return false;

	auto plCol = dynamic_cast<ColliderCapsule*>(
		player->GetCollider((int)COLLIDER_TYPE::CAPSULE));

	if (!plCol) return false;

	return myCol->IsHit(plCol);
}

ColliderBase* EnemyBase::GetCollider(int type)
{
	if (ownColliders_.count(type) == 0)
	{
		return nullptr;
	}

	return ownColliders_.at(type);
}

int EnemyBase::GetHp() const
{
	return hp_;
}

void EnemyBase::UpdateIdle()
{
	movePow_ = AsoUtility::VECTOR_ZERO;

	// プレイヤー発見
	if (SearchPlayer())
	{
		state_ = STATE::CHASE;
	}
}

void EnemyBase::UpdateChase()
{
	if (target_ == nullptr)return;

	// プレイヤー方向
	VECTOR dir = GetPlayerDirection();

	// 向き
	moveDir_ = dir;

	// 移動量
	movePow_ = VScale(dir, moveSpeedEnemy_);

	// 攻撃距離
	if (IsHit(target_))
	{
		target_->Damage(999);

		state_ = STATE::ATTACK;
	}
}

void EnemyBase::UpdateDashReady()
{
	movePow_ = AsoUtility::VECTOR_ZERO;

	dashDir_ = GetPlayerDirection();
	moveDir_ = dashDir_;

	dashTimer_++;

	if (dashTimer_ >= 180)
	{

		

		if (chargeEffectPlaying_)
		{
			StopEffekseer3DEffect(chargeEffectHandle_);
			chargeEffectPlaying_ = false;
			chargeEffectHandle_ = -1;
		}

		dashTimer_ = 0;

		dashSpeed_ = 150.0f;

		tackleTimer_ = 50;

		isTackle_ = true;

		animationController_->Play(2, true);
		state_ = STATE::DASH;
	}
}

void EnemyBase::UpdateDash()
{
	moveDir_ = dashDir_;
	movePow_ = VScale(dashDir_, dashSpeed_);

	dashSpeed_ *= 0.95f;


	// 突進中の当たり判定
	if (target_ && IsHit(target_))
	{
		target_->Damage(999);
	}

	tackleTimer_--;

	if (tackleTimer_ <= 0)
	{
		isTackle_ = false;

		movePow_ = AsoUtility::VECTOR_ZERO;
		animationController_->Play(1, true);
		state_ = STATE::CHASE;
	}
}

void EnemyBase::UpdateAttack()
{
	// 攻撃中は止まる
	movePow_ = AsoUtility::VECTOR_ZERO;
}

void EnemyBase::UpdateDamage()
{
}

void EnemyBase::UpdateDead()
{
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 死亡アニメーションが終わったら消去対象にする
	if (IsDeadAnimationEnd())
	{
		isDead_ = true;
	}
}

VECTOR EnemyBase::GetPlayerDirection() const
{
	if (target_ == nullptr)
	{
		return AsoUtility::VECTOR_ZERO;
	}

	VECTOR dir =
		VSub(target_->GetPos(), transform_.pos);

	// 高さ無視
	dir.y = 0.0f;

	// 長さチェック
	if (VSize(dir) <= 0.001f)
	{
		return AsoUtility::VECTOR_ZERO;
	}

	return VNorm(dir);
}

float EnemyBase::GetPlayerDistance() const
{
	if (target_ == nullptr)
	{
		return 999999.0f;
	}

	return VSize(
		VSub(target_->GetPos(), transform_.pos));
}

bool EnemyBase::SearchPlayer() const
{
	return GetPlayerDistance() < searchRange_;
}

void EnemyBase::Damage(int power)
{
	// 死亡演出中は無視
	if (state_ == STATE::DEAD) return;

	hp_ -= power;

	state_ = STATE::DAMAGE;

	if (hp_ <= 0)
	{
		hp_ = 0;

		state_ = STATE::DEAD;
	}
}
