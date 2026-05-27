#include "../../../Manager/ResourceManager.h"
#include "Door.h"

Door::Door(void)
{
}

Door::~Door(void)
{
}

void Door::Init(void)
{
	InitLoad();
	InitTransform();
	InitPost();
}

void Door::Update(void)
{
	if (isOpen_)
	{
		if (slideAmount_ < 100.0f)
		{
			slideAmount_ += 2.0f;
		}
	}
	else
	{
		if (slideAmount_ > 0.0f)
		{
			slideAmount_ -= 2.0f;
		}
	}

	leftPos_.x = -50.0f - slideAmount_;
	rightPos_.x = 50.0f + slideAmount_;
}

void Door::Draw(void)
{
	MV1SetPosition(leftHandle_, leftPos_);
	MV1DrawModel(leftHandle_);

	MV1SetPosition(rightHandle_, rightPos_);
	MV1DrawModel(rightHandle_);
}

void Door::Release(void)
{
}

void Door::Open(void)
{
	isOpen_ = true;
}

void Door::InitLoad(void)
{
	//ÉÇÉfÉãÇÃì«Ç›çûÇ›
	leftHandle_ =
		resMng_.Load(
			ResourceManager::SRC::DOOR_LEFT).handleId_;

	rightHandle_ =
		resMng_.Load(
			ResourceManager::SRC::DOOR_RIGHT).handleId_;
}

void Door::InitTransform(void)
{
	leftPos_ = VGet(50.0f, -9.0f, 0.0f);
	rightPos_ = VGet(50.0f, -9.0f, 0.0f);
}

void Door::InitCollider(void)
{
}

void Door::InitAnimation(void)
{
}

void Door::InitPost(void)
{
}
