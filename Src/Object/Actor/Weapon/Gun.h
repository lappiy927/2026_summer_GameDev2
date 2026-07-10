#pragma once
#include "WeaponBase.h"
#include "../../Collider/ColliderCapsule.h"

class Gun : public WeaponBase
{
public:

    Gun(void);
    ~Gun(void);

    void Init(void) override;
    void Update(const Transform& playerTransform, WEAPON_STATE weaponState) override;
    void Draw(void)override;

    ColliderCapsule* GetCollider(void) const override;

    //アニメーションのパスを取得
    const char* GetIdleAnimPath(void)      const override { return "Charactor/Player/Idle.mv1"; }
    const char* GetRunAnimPath(void)       const override { return "Charactor/Player/Run.mv1"; }
    const char* GetFastRunAnimPath(void)   const override { return "Charactor/Player/FastRun.mv1"; }
    const char* GetJumpAnimPath(void)      const override { return "Charactor/Player/Jump.mv1"; }
    const char* GetAttackAnimPath(void)    const override { return "Charactor/Player/Shot.mv1"; }
    const char* GetReloadAnimPath(void)    const override { return "Charactor/Player/Reload.mv1"; }
    const char* GetSwitchOutAnimPath(void) const override { return "Charactor/Player/GunIn.mv1"; }
    const char* GetSwitchInAnimPath(void)  const override { return "Charactor/Player/GunOut.mv1"; }

    float GetAttackDuration(void) const override { return 0.6f; }

    //残弾数の取得
    static int GetRemainingBullets(void) { return remainingBullets_; }

    //残弾数のリセット
    static void ResetRemainingBullets(int count = MAX_BULLET_COUNT) { remainingBullets_ = count; }


private:

    void InitLoad(void)      override;
    void InitTransform(void) override;
    void InitCollider(void)  override;

    void UpdateIdle(void);
    void UpdateMove(void);
    void UpdateDash(void);
    void UpdateAttack(const Transform& playerTransform);
    void UpdateJump(void);
    void UpdateReload(void);
    void UpdateTransform(const Transform& playerTransform);

    // 弾の発射処理
    void FireBullet(const Transform& playerTransform);

    // 弾の更新処理
    void UpdateBullet(void);

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

    //最大弾数
    static constexpr int MAX_BULLET_COUNT = 5;

    //残弾数
    static int remainingBullets_;

    int bulletCountHandle_ = -1;

    int bulletCountFontHandle_ = -1;

    // 残弾数表示のフォントサイズ
    static constexpr int BULLET_COUNT_FONT_SIZE = 36;

    //前フレームの武器の状態
    WEAPON_STATE prevWeaponState_ = WEAPON_STATE::IDLE;

    static constexpr float RAY_LENGTH = 1500.0f;
    static constexpr float ATTACK_HIT_DURATION = 0.5f;
    float attackTimer_ = 0.0f;
    bool isAttack_ = false;

    static constexpr WeaponOffset OFFSET_IDLE = { { 3.0f, 0.0f, -7.0f}, {DX_PI_F / 2.0f, DX_PI_F, 0.0f} };
    static constexpr WeaponOffset OFFSET_MOVE = { {0.0f,0.0f,0.0f}, {DX_PI_F / 2,DX_PI_F / 4,DX_PI_F} };
    static constexpr WeaponOffset OFFSET_DASH = { {0.0f,0.0f,0.0f}, {0.0f,DX_PI_F,0.0f} };
    static constexpr WeaponOffset OFFSET_ATTACK = { { 0.0f,0.0f,0.0f}, { 0.0f,DX_PI_F,-DX_PI_F / 4} };
    static constexpr WeaponOffset OFFSET_JUMP = { { 3.0f, 0.0f, -7.0f}, {DX_PI_F / 2.0f, DX_PI_F, 0.0f} };

    WeaponOffset currentOffset_ = OFFSET_IDLE;
};

