#pragma once
#include "../ActorBase.h"
#include "../../Common/Transform.h"

class ResourceManager;
class SceneManager;
class Player;

class WeaponBase
{
public:

	//コンストラクタ
	WeaponBase(Player* player);

	//デストラクタ
	~WeaponBase(void);

	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void);
	virtual void Release(void);

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;


	Player* player_;

};