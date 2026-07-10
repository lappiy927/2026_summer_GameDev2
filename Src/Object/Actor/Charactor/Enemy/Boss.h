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

	void AI() override;

	void Damage(int damage) override;

	ColliderCapsule* GetAttackCollider()const
	{
		return attackCollider_;
	}

	bool IsAttack() const;

	void UpdateAttackCollider();

private:

	ColliderCapsule* attackCollider_;

	bool isAttackHit_;

	bool attackEnable_;

	bool isAttacking_ = false;

	int leftHandFrame_ = -1;
};

