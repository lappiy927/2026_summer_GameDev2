#pragma once
#include "WeaponBase.h"
#include "../../Collider/ColliderCapsule.h"

class Katana : public WeaponBase
{
public:

    Katana(void);
    ~Katana(void);

    void Init(void) override;
    void Update(const Transform& playerTransform, WEAPON_STATE weaponState) override;

    ColliderCapsule* GetCollider(void) const override;

    //アニメーションのパスを取得
    const char* GetIdleAnimPath(void)const override { return "Charactor/Player/Idle.mv1"; }
    const char* GetRunAnimPath(void)const override { return "Charactor/Player/Run.mv1"; }
    const char* GetFastRunAnimPath(void)const override { return "Charactor/Player/FastRun.mv1"; }
    const char* GetJumpAnimPath(void)const override { return "Charactor/Player/Jump.mv1"; }
    const char* GetAttackAnimPath(void)const override { return "Charactor/Player/Attack.mv1"; }
    const char* GetReloadAnimPath(void)const override { return "Charactor/Player/Reload.mv1"; }
    const char* GetSwitchOutAnimPath(void)const override { return "Charactor/Player/KatanaIn.mv1"; }
    const char* GetSwitchInAnimPath(void)const override { return "Charactor/Player/KatanaIn.mv1"; }

    float GetAttackDuration(void) const override { return 1.0f; }

private:

    void InitLoad(void)      override;
    void InitTransform(void) override;
    void InitCollider(void)  override;

    void UpdateIdle(void);
    void UpdateMove(void);
    void UpdateDash(void);
    void UpdateAttack(void);
    void UpdateJump(void);
    void UpdateTransform(const Transform& playerTransform);

    static constexpr WeaponOffset OFFSET_IDLE = { { 3.0f, 0.0f, -7.0f}, {DX_PI_F / 2.0f, DX_PI_F, 0.0f          } };
    static constexpr WeaponOffset OFFSET_MOVE = { {-1.0f, 0.0f, -7.0f}, {DX_PI_F / 2.0f, DX_PI_F, DX_PI_F / 4.0f} };
    static constexpr WeaponOffset OFFSET_ATTACK = { { 3.0f, 0.0f, -5.0f}, {DX_PI_F / 4.0f, DX_PI_F, DX_PI_F / 4.0f} };
    static constexpr WeaponOffset OFFSET_JUMP = { { 3.0f, 0.0f, -7.0f}, {DX_PI_F / 2.0f, DX_PI_F, 0.0f          } };

    WEAPON_STATE weapon = WEAPON_STATE::IDLE;
    WeaponOffset currentOffset_ = OFFSET_IDLE;
};