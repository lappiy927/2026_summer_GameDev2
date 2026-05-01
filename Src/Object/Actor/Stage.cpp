#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
#include "../../Object/Collider/ColliderModel.h"
#include "Stage.h"

Stage::Stage(void)
	:
	ActorBase()
{
}

Stage::~Stage(void)
{
}

void Stage::Update(void)
{
}

void Stage::InitLoad(void)
{
	//モデルの読み込み
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::MAIN_STAGE).handleId_);
}

void Stage::InitTransform(void)
{
	transform_.scl = VGet(0.5f,0.5f,0.5f);
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.pos = DEFAULT_POS;
	transform_.Update();
}

void Stage::InitCollider(void)
{
	// DxLib側の衝突情報セットアップ
	MV1SetupCollInfo(transform_.modelId);

	// モデルのコライダ
	ColliderModel* colModel =
		new ColliderModel(ColliderBase::TAG::STAGE, &transform_);
	for (const std::string& name : EXCLUDE_FRAME_NAMES)
	{
		colModel->AddExcludeFrameIds(name);
	}

	for (const std::string& name : TARGET_FRAME_NAMES)
	{
		colModel->AddTargetFrameIds(name);
	}

	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::MODEL), colModel);
}

void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}
