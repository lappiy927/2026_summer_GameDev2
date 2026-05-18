#pragma once
#include "WeaponBase.h"
#include "../../Collider/ColliderCapsule.h"

// オフセット構造体を追加
struct WeaponOffset
{
    VECTOR localPos;
    VECTOR rotEuler; // オイラー角(ラジアン)
};

class Katana : public WeaponBase
{
public:

    //コンストラクタ
    Katana(Player* player);

    //デストラクタ
    ~Katana(void);

    void Init(void)override;
    void Update(void)override;

    ColliderCapsule* GetCollider() const;

private:

    // リソースロード
    void InitLoad(void) override;

    // 大きさ、回転、座標の初期化
    void InitTransform(void) override;

    // 衝突判定の初期化
    void InitCollider(void) override;

    void UpdateIdle(void)override;
    void UpdateMove(void)override;
    void UpdateDash(void)override;
    void UpdateAttack(void)override;

    //刀の追従用処理
    void UpdateTransform(void);

    ColliderCapsule* attackCollider_ = nullptr;

    bool isAttack_ = false;

    float attackTimer_ = 0.0f;

    bool oldMouse_ = false;

    // 状態ごとのオフセット定数
    static constexpr WeaponOffset OFFSET_IDLE = { {0.0f, 300.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
    static constexpr WeaponOffset OFFSET_MOVE = { {2.0f,  300.0f, -5.0f}, {DX_PI_F / 2.0f,DX_PI_F, 0.0f} };
    static constexpr WeaponOffset OFFSET_ATTACK = { {0.0f, 300.0f,10.0f}, {DX_PI_F / 4.f,DX_PI_F, 0.0f} };

    // 現在のオフセット
    WeaponOffset currentOffset_ = OFFSET_IDLE;

};