#pragma once
#include "EnemyBase.h"

class Player;
class ColliderCapsule;

class Boss :
	public EnemyBase
{
public:

	Boss();
	virtual ~Boss();

	void InitLoad() override;
	void InitTransform() override;
	void InitCollider() override;
	void InitAnimation() override;
	void InitPost() override;

	// ìGAI
	void AI() override;

	// É_ÉÅÅ[ÉW
	void Damage(int damage) override;

	ColliderCapsule* GetAttackCollider()const
	{
		return attackCollider_;
	}

	// çUåÇíÜÇ©Ç«Ç§Ç©
	bool IsAttack() const;

	void UpdateAttackCollider();

private:
	static constexpr int BOSS_HP = 500;
	static constexpr float ATTACK_RANGE = 400.0f;
	static constexpr float SEARCH_RANGE = 3000.0f;

	ColliderCapsule* attackCollider_;

	bool isAttackHit_;

	bool attackEnable_;

	bool isAttacking_;

	int leftHandFrame_;

	int chargeEffectHandle_;
	bool chargeEffectPlaying_ = false;

	int chargeEffect_;
};