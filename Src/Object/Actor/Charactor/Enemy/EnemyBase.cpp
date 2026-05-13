#include "EnemyBase.h"
#include "../Player.h"
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Object/Collider/ColliderCapsule.h"

EnemyBase::EnemyBase()
	:
	CharactorBase(),
	state_(STATE::IDLE),
	target_(nullptr),
	hp_(100),
	isDead_(false),
	searchRange_(300.0f),
	attackRange_(50.0f),
	moveSpeedEnemy_(3.0f)
{
}

EnemyBase::~EnemyBase()
{
}

void EnemyBase::UpdateProcess()
{
	//AI
	AI();

	// 状態更新
	switch (state_)
	{
	case STATE::IDLE:
		UpdateIdle();
		break;

	case STATE::CHASE:
		UpdateChase();
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
	return false;
}

void EnemyBase::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

bool EnemyBase::IsHit(Player* player)
{
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

void EnemyBase::UpdateAttack()
{
	// 停止
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 離れたら追跡
	if (GetPlayerDistance() > attackRange_)
	{
		state_ = STATE::CHASE;
	}
}

void EnemyBase::UpdateDamage()
{
}

void EnemyBase::UpdateDead()
{
	movePow_ = AsoUtility::VECTOR_ZERO;
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
	hp_ -= power;

	state_ = STATE::DAMAGE;

	if (hp_ <= 0)
	{
		hp_ = 0;

		isDead_ = true;

		state_ = STATE::DEAD;
	}
}
