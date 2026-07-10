#include <DxLib.h>
#include "../Object/Common/Transform.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Weapon/Katana.h"
#include "../Object/Actor/Weapon/Gun.h"
#include "../Object/Collider/ColliderCapsule.h"
#include "WeaponManager.h"



WeaponManager::WeaponManager(void)
    :
    katana_(nullptr),
    gun_(nullptr),
    activeType_(WEAPON_TYPE::KATANA)
{
}

WeaponManager::~WeaponManager(void)
{
}

void WeaponManager::Init(void)
{
    katana_ = new Katana();
    katana_->Init();

    gun_ = new Gun();
    gun_->Init();
}

void WeaponManager::Update(const Transform& playerTransform, WEAPON_STATE weaponState)
{
    switch (activeType_)
    {
    case WEAPON_TYPE::KATANA:
        katana_->Update(playerTransform, weaponState);
        UpdateHolster(gun_, HOLSTER_BONE, HOLSTER_GUN_POS, HOLSTER_GUN_ROT, playerTransform);
        break;
    case WEAPON_TYPE::GUN:
        gun_->Update(playerTransform, weaponState);
        UpdateHolster(katana_, HOLSTER_BONE, HOLSTER_KATANA_POS, HOLSTER_KATANA_ROT, playerTransform);
        break;
    }
}

void WeaponManager::Draw(void)
{
    katana_->Draw();
    gun_->Draw();
}

void WeaponManager::Release(void)
{
    if (katana_ != nullptr)
    {
        katana_->Release();
        delete katana_;
        katana_ = nullptr;
    }
    if (gun_ != nullptr)
    {
        gun_->Release();
        delete gun_;
        gun_ = nullptr;
    }
}

void WeaponManager::SetActiveWeapon(WEAPON_TYPE type)
{
    activeType_ = type;
}

WeaponManager::WEAPON_TYPE WeaponManager::GetActiveWeaponType(void) const
{
    return activeType_;
}

ColliderCapsule* WeaponManager::GetActiveCollider(void) const
{
    return GetActiveWeapon()->GetCollider();
}

WeaponManager::SwitchResult WeaponManager::ApplyWeaponSwitch(void)
{
    activeType_ = (activeType_ == WEAPON_TYPE::KATANA)
        ? WEAPON_TYPE::GUN
        : WEAPON_TYPE::KATANA;

    return SwitchResult::BEGIN_SWITCH_IN;
}

WeaponBase* WeaponManager::GetActiveWeapon(void) const
{
    return (activeType_ == WEAPON_TYPE::KATANA)
        ? static_cast<WeaponBase*>(katana_)
        : static_cast<WeaponBase*>(gun_);
}

const char* WeaponManager::GetIdleAnimPath(void)      const
{
    return GetActiveWeapon()->GetIdleAnimPath();
}

const char* WeaponManager::GetRunAnimPath(void)       const
{
    return GetActiveWeapon()->GetRunAnimPath();
}

const char* WeaponManager::GetFastRunAnimPath(void)   const
{
    return GetActiveWeapon()->GetFastRunAnimPath();
}

const char* WeaponManager::GetJumpAnimPath(void)      const
{
    return GetActiveWeapon()->GetJumpAnimPath();
}

const char* WeaponManager::GetAttackAnimPath(void)    const
{
    return GetActiveWeapon()->GetAttackAnimPath();
}

const char* WeaponManager::GetReloadAnimPath(void)    const
{
    return GetActiveWeapon()->GetReloadAnimPath();
}

const char* WeaponManager::GetSwitchOutAnimPath(void) const
{
    return GetActiveWeapon()->GetSwitchOutAnimPath();
}

const char* WeaponManager::GetSwitchInAnimPath(void)  const
{
    return GetActiveWeapon()->GetSwitchInAnimPath();
}

float WeaponManager::GetAttackDuration(void)    const
{
    return GetActiveWeapon()->GetAttackDuration();
}

void WeaponManager::UpdateHolster(WeaponBase* weapon, const char* boneName,
    VECTOR localPos, VECTOR rotEuler,
    const Transform& playerTransform)
{
    int playerModelId = playerTransform.modelId;
    int boneFrame = MV1SearchFrame(playerModelId, boneName);
    if (boneFrame == -1) return;

    MATRIX boneMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, boneFrame);

    VECTOR bonePos = VGet(
        boneMatrix.m[3][0],
        boneMatrix.m[3][1],
        boneMatrix.m[3][2]);

    MATRIX boneRotOnly = MGetIdent();
    VECTOR col0 = VNorm(VGet(boneMatrix.m[0][0], boneMatrix.m[0][1], boneMatrix.m[0][2]));
    VECTOR col1 = VNorm(VGet(boneMatrix.m[1][0], boneMatrix.m[1][1], boneMatrix.m[1][2]));
    VECTOR col2 = VNorm(VGet(boneMatrix.m[2][0], boneMatrix.m[2][1], boneMatrix.m[2][2]));
    boneRotOnly.m[0][0] = col0.x; boneRotOnly.m[0][1] = col0.y; boneRotOnly.m[0][2] = col0.z;
    boneRotOnly.m[1][0] = col1.x; boneRotOnly.m[1][1] = col1.y; boneRotOnly.m[1][2] = col1.z;
    boneRotOnly.m[2][0] = col2.x; boneRotOnly.m[2][1] = col2.y; boneRotOnly.m[2][2] = col2.z;
    boneRotOnly.m[3][0] = 0.0f;   boneRotOnly.m[3][1] = 0.0f;   boneRotOnly.m[3][2] = 0.0f;

    MATRIX offsetRot = Quaternion::Euler(rotEuler).ToMatrix();
    MATRIX rotMatrix = MMult(offsetRot, boneRotOnly);

    VECTOR offset = VTransform(localPos, rotMatrix);
    VECTOR finalPos = VAdd(bonePos, offset);

    MATRIX finalMatrix = rotMatrix;
    finalMatrix.m[3][0] = finalPos.x;
    finalMatrix.m[3][1] = finalPos.y;
    finalMatrix.m[3][2] = finalPos.z;

    MV1SetMatrix(weapon->GetTransform().modelId, finalMatrix);

    ColliderCapsule* col = weapon->GetCollider();
    if (col != nullptr)
    {
        col->SetEnable(false);
        col->SetLocalPosTop(finalPos);
        col->SetLocalPosDown(finalPos);
    }
}