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
};

