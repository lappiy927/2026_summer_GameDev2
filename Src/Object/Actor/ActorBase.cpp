#include "../../Manager/ResourceManager.h"
#include "../../Manager/SceneManager.h"
#include "ActorBase.h"

ActorBase::ActorBase(void)
	:
	resMng_(ResourceManager::GetInstance()),
	scnMng_(SceneManager::GetInstance()),
	transform_()
{
}

ActorBase::~ActorBase(void)
{
}

void ActorBase::Init(void)
{
	// リソースロード
	InitLoad();

	// Transform初期化
	InitTransform();

	// アニメーションの初期化
	InitAnimation();

	// 初期化後の個別処理
	InitPost();
}

void ActorBase::Draw(void)
{
	if (transform_.modelId != -1)
	{
		MV1DrawModel(transform_.modelId);
	}
}

void ActorBase::Release(void)
{
	transform_.Release();
}

const Transform& ActorBase::GetTransform(void) const
{
	return transform_;
}
