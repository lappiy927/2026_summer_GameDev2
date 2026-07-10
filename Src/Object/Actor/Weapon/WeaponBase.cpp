#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Collider/ColliderCapsule.h"
#include "WeaponBase.h"

WeaponBase::WeaponBase(void)
    :
    resMng_(ResourceManager::GetInstance()),
    scnMng_(SceneManager::GetInstance()),
    transform_()
{
}

WeaponBase::~WeaponBase(void)
{
}

void WeaponBase::Draw(void)
{
    if (transform_.modelId != -1)
    {
        MV1DrawModel(transform_.modelId);
    }

    dynamic_cast<ColliderCapsule*>(attackCollider_)->DrawDebug(0xff0000);
}

void WeaponBase::Release(void)
{
    transform_.Release();

    DeleteEffekseerEffect(effectHandle);

    effectHandle = -1;

    if (attackCollider_ != nullptr)
    {
        delete attackCollider_;
        attackCollider_ = nullptr;
    }
}