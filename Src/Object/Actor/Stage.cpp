#include "../../Utility/AsoUtility.h"
#include "../../Manager/ResourceManager.h"
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
	//ÉÇÉfÉãÇÃì«Ç›çûÇ›
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::MAIN_STAGE).handleId_);
}

void Stage::InitTransform(void)
{
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.pos = DEFAULT_POS;
	transform_.Update();
}


void Stage::InitAnimation(void)
{
}

void Stage::InitPost(void)
{
}
