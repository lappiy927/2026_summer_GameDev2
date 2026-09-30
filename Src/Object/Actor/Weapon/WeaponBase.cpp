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
	transform_(),
	attackCollider_(nullptr),
	effectHandle_(-1)
{
}

WeaponBase::~WeaponBase(void)
{
}

void WeaponBase::Draw(void)
{
	if (transform_.modelId_ != -1)
	{
		MV1DrawModel(transform_.modelId_);
	}
}

void WeaponBase::Release(void)
{
	transform_.Release();

	DeleteEffekseerEffect(effectHandle_);

	effectHandle_ = -1;

	if (attackCollider_ != nullptr)
	{
		delete attackCollider_;
		attackCollider_ = nullptr;
	}
}