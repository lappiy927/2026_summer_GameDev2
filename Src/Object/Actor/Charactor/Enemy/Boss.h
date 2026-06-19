#pragma once
#include "EnemyBase.h"

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

	bool IsDead() const override;

	bool IsDeadAnimationEnd() const;

private:

	int hp_ = 50;
	bool isDead_ = false;
};

