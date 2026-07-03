#include <DxLib.h>
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Weapon/Katana.h"
#include "../Object/Actor/Weapon/Gun.h"
#include "../Object/Collider/ColliderCapsule.h"
#include "WeaponManager.h"

WeaponManager::WeaponManager(Player* player)
    :
    player_(player),
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
    katana_ = new Katana(player_);
    katana_->Init();

    gun_ = new Gun(player_);
    gun_->Init();

    activeType_ = WEAPON_TYPE::KATANA;
}

void WeaponManager::Update(void)
{
    switch (activeType_)
    {
    case WEAPON_TYPE::KATANA:
        katana_->Update();
        UpdateHolster(gun_, HOLSTER_BONE, HOLSTER_GUN_POS, HOLSTER_GUN_ROT);
        break;

    case WEAPON_TYPE::GUN:
        gun_->Update();
        UpdateHolster(katana_, HOLSTER_BONE, HOLSTER_KATANA_POS, HOLSTER_KATANA_ROT);
        break;
    }
}

void WeaponManager::Draw(void)
{
    // 両武器を常に描画（非アクティブも背中に表示）
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
    switch (activeType_)
    {
    case WEAPON_TYPE::KATANA: return katana_->GetCollider();
    case WEAPON_TYPE::GUN:    return gun_->GetCollider();
    }
    return nullptr;
}

void WeaponManager::UpdateHolster(WeaponBase* weapon, const char* boneName, VECTOR localPos, VECTOR rotEuler)
{
    int playerModelId = player_->GetTransform().modelId;
    int boneFrame = MV1SearchFrame(playerModelId, boneName);
    if (boneFrame == -1) return;

    MATRIX boneMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, boneFrame);

    VECTOR bonePos = VGet(
        boneMatrix.m[3][0],
        boneMatrix.m[3][1],
        boneMatrix.m[3][2]);

    // スケール除去した純粋な回転行列を作成
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
