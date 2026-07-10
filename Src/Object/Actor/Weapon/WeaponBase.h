#pragma once
#include "../ActorBase.h"
#include "../../Common/Transform.h"
#include "WeaponState.h"

class ResourceManager;
class SceneManager;
class ColliderCapsule;
class ColliderBase;

struct WeaponOffset
{
    VECTOR localPos;
    VECTOR rotEuler;
};

class WeaponBase
{
public:

    WeaponBase(void);
    ~WeaponBase(void);

    virtual void Init(void) = 0;
    virtual void Update(const Transform& playerTransform, WEAPON_STATE weaponState) = 0;
    virtual void Draw(void);
    virtual void Release(void);

    const Transform& GetTransform(void) const { return transform_; }

    virtual ColliderCapsule* GetCollider(void) const = 0;

    //アニメーションのパスを取得
    virtual const char* GetIdleAnimPath(void)const = 0;
    virtual const char* GetRunAnimPath(void)const = 0;
    virtual const char* GetFastRunAnimPath(void)const = 0;
    virtual const char* GetJumpAnimPath(void)const = 0;
    virtual const char* GetAttackAnimPath(void)const = 0;
    virtual const char* GetReloadAnimPath(void)const = 0;
    virtual const char* GetSwitchOutAnimPath(void)const = 0;
    virtual const char* GetSwitchInAnimPath(void)const = 0;

    virtual float GetAttackDuration(void) const = 0;

protected:

    ResourceManager& resMng_;
    SceneManager& scnMng_;

    Transform transform_;
    ColliderCapsule* attackCollider_ = nullptr;

    virtual void InitLoad(void) = 0;
    virtual void InitTransform(void) = 0;
    virtual void InitCollider(void) = 0;

    int effectHandle;
};