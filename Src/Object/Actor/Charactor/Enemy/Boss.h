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
};

