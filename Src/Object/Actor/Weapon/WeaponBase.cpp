#include<DxLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "WeaponBase.h"

WeaponBase::WeaponBase(Player* player)
	:
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	transform_(),
	player_(player)
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
}

void WeaponBase::Release(void)
{
	transform_.Release();
}