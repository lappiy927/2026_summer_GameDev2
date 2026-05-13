#pragma once
#include "EnemyBase.h"

class EnemyMob : public EnemyBase
{
public:

	EnemyMob();
	virtual ~EnemyMob();

	 void InitLoad() override;
	 void InitTransform() override;
	 void InitCollider() override;
	 void InitAnimation() override;
	 void InitPost() override;

	 void AI() override;

	 void Damage(int damage) override;

	 bool IsDead() const override;

private:

	int hp_ = 100;
	bool isDead_ = false;
};

