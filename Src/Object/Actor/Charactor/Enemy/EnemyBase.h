#pragma once
#include "../CharactorBase.h"

class Player;

class EnemyBase :
	public CharactorBase
{
public:

	enum class STATE
	{
		IDLE,
		CHASE,
		DASH_READY,
		DASH,
		ATTACK,
		DAMAGE,
		DEAD
	};

	EnemyBase();
	virtual ~EnemyBase();

	virtual void UpdateProcess() override;
	virtual void UpdateProcessPost() override;

	// 敵AI
	virtual void AI() = 0;

	// ターゲット設定
	void SetTarget(Player* player);

	// 死亡確認
	virtual bool IsDead() const;

	void SetPos(const VECTOR& pos);

	bool IsHit(Player* player);

	// 死亡アニメーション終了確認
	bool IsDeadAnimationEnd() const;

	// ダメージ
	virtual void Damage(int power);

	ColliderBase* GetCollider(int type);

	int GetHp() const;

protected:

	// 状態
	STATE state_;

	// プレイヤー
	Player* target_;

	// HP
	int hp_;

	// 死亡フラグ
	bool isDead_;

	// 索敵距離
	float searchRange_;

	// 攻撃距離
	float attackRange_;

	// 敵移動速度
	float moveSpeedEnemy_;

	VECTOR effectPos_ = AsoUtility::VECTOR_ZERO;

protected:

	// 状態更新
	void UpdateIdle();
	void UpdateChase();
	void UpdateDashReady();
	void UpdateDash();
	void UpdateAttack();
	void UpdateDamage();
	void UpdateDead();

	// プレイヤー方向
	VECTOR GetPlayerDirection() const;

	// プレイヤー距離
	float GetPlayerDistance() const;

	// プレイヤー発見
	bool SearchPlayer() const;

	int effectHandle_;

	VECTOR dashDir_;

	float dashSpeed_;

	int dashTimer_;

	bool isLanding_;

	bool isTackle_;

	int tackleTimer_;

	//チャージエフェクト
	int chargeEffectHandle_;
	bool chargeEffectPlaying_;

	int chargeEffect_;
};