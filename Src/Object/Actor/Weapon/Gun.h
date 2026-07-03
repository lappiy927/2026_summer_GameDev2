#pragma once
#include "WeaponBase.h"
#include "../../Collider/ColliderCapsule.h"

class Gun : public WeaponBase
{
public:

    Gun(Player* player);
    ~Gun(void);

    void Init(void)override;
    void Update(void)override;
    void Draw(void) override;

    ColliderCapsule* GetCollider() const;

private:

    void InitLoad(void) override;
    void InitTransform(void) override;
    void InitCollider(void) override;

    void UpdateIdle(void)override;
    void UpdateMove(void)override;
    void UpdateDash(void)override;
    void UpdateAttack(void)override;
    void UpdateJump(void)override;
    void UpdateReload(void);


    void UpdateTransform(void);

    // 弾の発射処理
    void FireBullet(void);

    // 弾の更新処理
    void UpdateBullet(void);

    ColliderCapsule* attackCollider_ = nullptr;

    // 弾の状態
    enum class BULLET_STATE
    {
        IDLE,       // 待機中（発射可能）
        FLYING,     // 飛翔中
        RELOADING,  // リロード中
    };

    BULLET_STATE bulletState_ = BULLET_STATE::IDLE;

    // 弾の現在座標
    VECTOR bulletPos_ = { 0.0f, 0.0f, 0.0f };

    // 弾の飛翔方向
    VECTOR bulletDir_ = { 0.0f, 0.0f, 1.0f };

    // 弾の発射原点（着弾判定用）
    VECTOR bulletOrigin_ = { 0.0f, 0.0f, 0.0f };

    // 弾の速度（1フレームあたりの移動量）
    static constexpr float BULLET_SPEED = 50.0f;

    // 弾の最大射程
    static constexpr float BULLET_MAX_RANGE = 1500.0f;

    // 弾の描画半径
    static constexpr float BULLET_RADIUS = 50.0f;

    // 前フレームのアニメ状態
    Player::ANIM_TYPE prevAnimType_ = Player::ANIM_TYPE::IDLE;

    static constexpr float RAY_LENGTH = 1500.0f;
    static constexpr float ATTACK_HIT_DURATION = 0.5f;
    float attackTimer_ = 0.0f;
    bool isAttack_ = false;

    static constexpr WeaponOffset OFFSET_IDLE = { {3.0f, 0.0f, -7.0f}, {0.0f, DX_PI_F, 0.0f} };
    static constexpr WeaponOffset OFFSET_MOVE = { {0.0f, 0.0f,  0.0f}, {0.0f, -DX_PI_F / 2.0f, 0.0f} };
    static constexpr WeaponOffset OFFSET_ATTACK = { {3.0f, 0.0f, -5.0f}, {DX_PI_F / 4.0f, DX_PI_F, DX_PI_F / 4.0f} };
    static constexpr WeaponOffset OFFSET_JUMP = { {3.0f, 0.0f, -7.0f}, {DX_PI_F / 2, DX_PI_F, 0.0f} };

    WeaponOffset currentOffset_ = OFFSET_IDLE;
};
