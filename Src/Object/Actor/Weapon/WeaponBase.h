#pragma once
#include "../ActorBase.h"
#include "../../Common/Transform.h"
#include "../../Collider/ColliderCapsule.h"

class ResourceManager;
class SceneManager;
class Player;

// オフセット構造体を追加
struct WeaponOffset
{
	VECTOR localPos;
	VECTOR rotEuler; // オイラー角(ラジアン)
};

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

	const Transform& GetTransform(void) const { return transform_; }

	// 派生クラスのattackCollider_を外部から取得
	virtual ColliderCapsule* GetCollider(void) const = 0;

protected:

	// シングルトン参照
	ResourceManager& resMng_;
	SceneManager& scnMng_;

	// モデル制御の基本情報
	Transform transform_;

	ColliderBase* attackCollider_ = nullptr;

	// リソースロード
	virtual void InitLoad(void) = 0;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) = 0;

	// 衝突判定の初期化
	virtual void InitCollider(void) = 0;

	//待機時の更新
	virtual void UpdateIdle(void) = 0;
	//移動(歩き)時の更新
	virtual void UpdateMove(void) = 0;
	//移動(ダッシュ)時の更新
	virtual void UpdateDash(void) = 0;
	//攻撃時の更新
	virtual void UpdateAttack(void) = 0;
	//ジャンプ時の更新
	virtual void UpdateJump(void) = 0;

	int effectHandle = -1;

	Player* player_;

};