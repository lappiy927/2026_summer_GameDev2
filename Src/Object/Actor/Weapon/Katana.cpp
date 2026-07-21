#include <DxLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Common/Transform.h"
#include "../../Collider/ColliderCapsule.h"
#include "Katana.h"

Katana::Katana(void)
    :
    WeaponBase()
{
}

Katana::~Katana(void)
{
}

void Katana::Init(void)
{
    InitLoad();
    InitTransform();
    InitCollider();
}

void Katana::Update(const Transform& playerTransform, WEAPON_STATE weaponState)
{
    attackCollider_->SetEnable(false);
  
    
    if (weaponState != weapon && weaponState == WEAPON_STATE::ATTACK) {
        attackCollider_->SetEnable(true);
    }

    switch (weaponState)
    {
    case WEAPON_STATE::IDLE:
        UpdateIdle();
        break;
    case WEAPON_STATE::MOVE:
        UpdateMove();
        break;
    case WEAPON_STATE::DASH:
        UpdateDash();
        break;
    case WEAPON_STATE::JUMP:
        UpdateJump();
        break;
    case WEAPON_STATE::ATTACK:
        UpdateAttack();
        break;
    }

    UpdateTransform(playerTransform);

     weapon = weaponState;

}

ColliderCapsule* Katana::GetCollider(void) const
{
    return dynamic_cast<ColliderCapsule*>(attackCollider_);
}

void Katana::InitLoad(void)
{
    transform_.SetModel(
        resMng_.Load(ResourceManager::SRC::KATANA).handleId_);
}

void Katana::InitTransform(void)
{
    transform_.scl = AsoUtility::VECTOR_ONE;
    transform_.quaRot = Quaternion::Identity();
    transform_.quaRotLocal = Quaternion::Identity();
    transform_.pos = VGet(0.0f, 0.0f, 0.0f);
    transform_.Update();
}

void Katana::InitCollider(void)
{
    attackCollider_ = new ColliderCapsule(
        ColliderBase::TAG::PLAYER_ATTACK,
        &transform_,
        VGet(0, 0, 100),
        VGet(0, 0, 300),
        20000.0f);
}

void Katana::UpdateIdle(void)
{
    currentOffset_ = OFFSET_IDLE;
}

void Katana::UpdateMove(void)
{
    currentOffset_ = OFFSET_MOVE;
}

void Katana::UpdateDash(void)
{
    currentOffset_ = OFFSET_MOVE;
}

void Katana::UpdateJump(void)
{
    currentOffset_ = OFFSET_JUMP;
}

void Katana::UpdateAttack(void)
{
    currentOffset_ = OFFSET_ATTACK;
    
}

void Katana::UpdateTransform(const Transform& playerTransform)
{
    int playerModelId = playerTransform.modelId;
    int rightHandFrame = MV1SearchFrame(playerModelId, "hand.R");
    if (rightHandFrame == -1) return;

    MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, rightHandFrame);

    VECTOR handPos = VGet(
        handMatrix.m[3][0],
        handMatrix.m[3][1],
        handMatrix.m[3][2]);

    MATRIX handRotOnly = MGetIdent();
    VECTOR col0 = VNorm(VGet(handMatrix.m[0][0], handMatrix.m[0][1], handMatrix.m[0][2]));
    VECTOR col1 = VNorm(VGet(handMatrix.m[1][0], handMatrix.m[1][1], handMatrix.m[1][2]));
    VECTOR col2 = VNorm(VGet(handMatrix.m[2][0], handMatrix.m[2][1], handMatrix.m[2][2]));
    handRotOnly.m[0][0] = col0.x; handRotOnly.m[0][1] = col0.y; handRotOnly.m[0][2] = col0.z;
    handRotOnly.m[1][0] = col1.x; handRotOnly.m[1][1] = col1.y; handRotOnly.m[1][2] = col1.z;
    handRotOnly.m[2][0] = col2.x; handRotOnly.m[2][1] = col2.y; handRotOnly.m[2][2] = col2.z;
    handRotOnly.m[3][0] = 0.0f;   handRotOnly.m[3][1] = 0.0f;   handRotOnly.m[3][2] = 0.0f;

    MATRIX offsetRot = Quaternion::Euler(currentOffset_.rotEuler).ToMatrix();
    MATRIX rotMatrix = MMult(offsetRot, handRotOnly);

    VECTOR rotatedTop = VTransform(VGet(0.0f, 0.0f, 0.0f), rotMatrix);
    VECTOR rotatedDown = VTransform(VGet(0.0f, 150.0f, 0.0f), rotMatrix);

    VECTOR offset = VTransform(currentOffset_.localPos, rotMatrix);
    VECTOR finalPos = VAdd(handPos, offset);

    MATRIX finalMatrix = rotMatrix;
    finalMatrix.m[3][0] = finalPos.x;
    finalMatrix.m[3][1] = finalPos.y;
    finalMatrix.m[3][2] = finalPos.z;

    MV1SetMatrix(transform_.modelId, finalMatrix);

    attackCollider_->SetLocalPosTop(VAdd(rotatedTop, finalPos));
    attackCollider_->SetLocalPosDown(VAdd(rotatedDown, finalPos));
}