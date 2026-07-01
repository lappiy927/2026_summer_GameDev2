#include <DxLib.h>
#include "../Application.h"
#include "Resource.h"
#include "ResourceManager.h"

ResourceManager* ResourceManager::instance_ = nullptr;

void ResourceManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new ResourceManager();
	}
	instance_->Init();
}

ResourceManager& ResourceManager::GetInstance(void)
{
	return *instance_;
}

void ResourceManager::Init(void)
{
	using RES = Resource;
	using RES_T = RES::TYPE;

	static std::string PATH_IMG = Application::PATH_IMAGE;
	static std::string PATH_MDL = Application::PATH_MODEL;
	static std::string PATH_EFF = Application::PATH_EFFECT;

	Resource* res;

	// MAIN_STAGEモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Stage/MainStage/MainStage.mv1");
	resourcesMap_.emplace(SRC::MAIN_STAGE, res);

	// BOSS_STAGEモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Stage/BossStage/BossRoom.mv1");
	resourcesMap_.emplace(SRC::BOSS_STAGE, res);

	// DOOR_LEFTモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Stage/MainStage/DoorLeft.mv1");
	resourcesMap_.emplace(SRC::DOOR_LEFT, res);

	// DOOR_RIGHTモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Stage/MainStage/DoorRight.mv1");
	resourcesMap_.emplace(SRC::DOOR_RIGHT, res);

	// DOOR_BACKモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Stage/MainStage/GateBack.mv1");
	resourcesMap_.emplace(SRC::DOOR_BACK, res);

	// PLAYERモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Charactor/Player/Player.mv1");
	resourcesMap_.emplace(SRC::PLAYER, res);

	// RUNモーション
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Charactor/Player/Run.mv1");
	resourcesMap_.emplace(SRC::PLAYER_RUN, res);

	// FAST_RUNモーション
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Charactor/Player/FastRun.mv1");
	resourcesMap_.emplace(SRC::PLAYER_FAST_RUN, res);

	// ATTACKモーション
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Charactor/Player/Attack.mv1");
	resourcesMap_.emplace(SRC::ATTACK, res);

	//KATANAモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Weapon/刀.mv1");
	resourcesMap_.emplace(SRC::KATANA, res);

	// ENEMYモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Charactor/Enemy/MobEnemy.mv1");
	resourcesMap_.emplace(SRC::ENEMY, res);

	// BOSSモデル
	res = new RES(Resource::TYPE::MODEL, PATH_MDL + "Charactor/Enemy/Boss/BossEnemy.mv1");
	resourcesMap_.emplace(SRC::BOSS, res);
}

void ResourceManager::Release(void)
{
	for (auto& p : loadedMap_)
	{
		p.second.Release();
	}

	loadedMap_.clear();
}

void ResourceManager::Destroy(void)
{
	Release();
	for (auto& res : resourcesMap_)
	{
		res.second->Release();
		delete res.second;
	}
	resourcesMap_.clear();
	delete instance_;
}

const Resource& ResourceManager::Load(SRC src)
{
	const Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return dummy_;
	}
	return res;
}

int ResourceManager::LoadModelDulicate(SRC src)
{
	Resource& res = _Load(src);
	if (res.type_ == Resource::TYPE::NONE)
	{
		return -1;
	}

	int duId = MV1DuplicateModel(res.handleId_);
	res.duplicateModelIds_.push_back(duId);

	return duId;
}

ResourceManager::ResourceManager(void)
{
}

Resource& ResourceManager::_Load(SRC src)
{

	// ロード済みチェック
	const auto& lPair = loadedMap_.find(src);
	if (lPair != loadedMap_.end())
	{
		return *resourcesMap_.find(src)->second;
	}

	// リソース登録チェック
	const auto& rPair = resourcesMap_.find(src);
	if (rPair == resourcesMap_.end())
	{
		// 登録されていない
		return dummy_;
	}

	// ロード処理
	rPair->second->Load();

	// 念のためコピーコンストラクタ
	loadedMap_.emplace(src, *rPair->second);

	return *rPair->second;

}
