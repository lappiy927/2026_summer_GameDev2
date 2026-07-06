#include "BossStage.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Collider/ColliderModel.h"
#include "../../../Renderer/VertexMaterial.h"
#include "../../../Renderer/VertexRenderer.h"

BossStage::BossStage(void)
{
}

BossStage::~BossStage(void)
{
}

void BossStage::Update(void)
{
}

void BossStage::Draw(void)
{
	renderer_->Draw(transform_.modelId);
}

void BossStage::InitLoad(void)
{
	//モデルの読み込み
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::BOSS_STAGE).handleId_);

	material_ = new VertexMaterial("VertexShader.cso", "PixelShader.cso", 1);
	auto dir = GetLightDirection();
	material_->AddConstBuf({ dir.x,dir.y, dir.z, 0.0f });
	renderer_ = new VertexRenderer(*material_);
}

void BossStage::InitTransform(void)
{
	transform_.scl = VGet(0.5f, 0.5f, 0.5f);
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.pos = VGet(2000.0f, 2900.0f, 2000.0f);
	transform_.Update();
}

void BossStage::InitCollider(void)
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

void BossStage::InitAnimation(void)
{
}

void BossStage::InitPost(void)
{
}
