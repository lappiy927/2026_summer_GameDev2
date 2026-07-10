#pragma once
#include "CharactorBase.h"
#include "../../Collider/ColliderCapsule.h"
#include "../Weapon/WeaponState.h"
class WeaponManager;

class Player : public CharactorBase
{
public:

    enum class ANIM_TYPE
    {
        IDLE,
        RUN,
        FAST_RUN,
        JUMP,
        ATTACK,
        RELOAD,
        SWITCH_OUT,
        SWITCH_IN,
    };

    Player(void);
    ~Player(void) override;

    ColliderBase* GetCollider(int type);
    void          SetPos(const VECTOR& pos);
    void          Damage(int power);
    bool          IsDead(void) const;

    void          SetWeaponManager(WeaponManager* weaponMng);

    ANIM_TYPE     animType_;
    ANIM_TYPE     GetAnimType(void) const;
    WEAPON_STATE  GetWeaponState(void) const;
    bool          IsSwitching(void) const;

protected:

    void InitLoad(void)      override;
    void InitTransform(void) override;
    void InitCollider(void)  override;
    void InitAnimation(void) override;
    void InitPost(void)      override;

    virtual void UpdateProcess(void);
    virtual void UpdateProcessPost(void);

private:

    static constexpr VECTOR COL_CAPSULE_TOP_JUMP_LOCAL_POS = { 0.0f, 160.0f, 0.0f };
    static constexpr VECTOR COL_CAPSULE_DOWN_JUMP_LOCAL_POS = { 0.0f,  80.0f, 0.0f };
    static constexpr VECTOR DEFAULT_ROT_LOCAL = { 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };

    static constexpr float  SPEED_MOVE = 5.0f;
    static constexpr float  SPEED_DASH = 20.0f;
    static constexpr float  POW_JUMP_INIT = 3500.0f;
    static constexpr float  POW_JUMP_KEEP = 400.0f;
    static constexpr float  TIME_JUMP_INPUT = 0.5f;
    static constexpr float  COL_CAPSULE_RADIUS = 20.0f;

    static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f,  80.0f, 0.0f };
    static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };
    static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };
    static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, -50.0f, 0.0f };
    static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };
    static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f,  30.0f, 0.0f };

    int  hp_ = 1;
    bool isDead_ = false;

    bool  isAttack_ = false;
    float attackTimer_ = 0.0f;

    bool isSwitching_ = false;

    bool isReload_ = false;
    float reloadTimer_ = 0.0f;

    static constexpr float RELOAD_ANIM_TIME = 1.5f;

    WeaponManager* weaponMng_ = nullptr;

    void ProcessMove(void);
    void ProcessJump(void);
    void ProcessAttack(void);
    void ProcessReload(void);
    void ProcessWeaponSwitch(void);
    void ProcessSwitch(void);

    void BeginSwitchOut(void);
    void BeginSwitchIn(void);
    void OnWeaponChanged(void);

    void CollisionReserve(void) override;
};