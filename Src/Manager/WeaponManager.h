#pragma once
#include <DxLib.h>
#include "../Object/Actor/Weapon/WeaponState.h"

class WeaponBase;
class Katana;
class Gun;
class ColliderCapsule;
class Transform;

class WeaponManager
{
public:

    enum class WEAPON_TYPE
    {
        KATANA,
        GUN,
    };

    enum class SwitchResult
    {
        NONE,
        BEGIN_SWITCH_IN,
    };

    WeaponManager(void);
    ~WeaponManager(void);

    void Init(void);
    void Update(const Transform& playerTransform, WEAPON_STATE weaponState);
    void Draw(void);
    void Release(void);

    void             SetActiveWeapon(WEAPON_TYPE type);
    WEAPON_TYPE      GetActiveWeaponType(void) const;
    ColliderCapsule* GetActiveCollider(void) const;

    SwitchResult ApplyWeaponSwitch(void);

    const char* GetIdleAnimPath(void)      const;
    const char* GetRunAnimPath(void)       const;
    const char* GetFastRunAnimPath(void)   const;
    const char* GetJumpAnimPath(void)      const;
    const char* GetAttackAnimPath(void)    const;
    const char* GetReloadAnimPath(void)    const;
    const char* GetSwitchOutAnimPath(void) const;
    const char* GetSwitchInAnimPath(void)  const;
    float       GetAttackDuration(void)    const;

private:

    void UpdateHolster(WeaponBase* weapon, const char* boneName,
        VECTOR localPos, VECTOR rotEuler,
        const Transform& playerTransform);

    WeaponBase* GetActiveWeapon(void) const;

    Katana* katana_;
    Gun* gun_;

    WEAPON_TYPE activeType_;

    static constexpr VECTOR HOLSTER_KATANA_POS = { -25.0f,-30.0f, 10.0f };
    static constexpr VECTOR HOLSTER_KATANA_ROT = { -DX_PI_F / 4.0f, 0.0f,DX_PI_F };
    static constexpr VECTOR HOLSTER_GUN_POS = { -25.0f, -50.0f, 0.0f };
    static constexpr VECTOR HOLSTER_GUN_ROT = { DX_PI_F,  DX_PI_F / 2.0f,0.0f };

    // é˚î[êÊÉ{Å[Éìñº
    static constexpr const char* HOLSTER_BONE = "spine";
};


