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
	// 開く
	if (isOpen_)
	{
		if (slideAmount_ < 150.0f)
		{
			slideAmount_ += 2.0f;
		}
	}

	// 基準位置へ戻す
	leftPos_ = leftBasePos_;
	rightPos_ = rightBasePos_;

	// 左右へ開く
	leftPos_.z -= slideAmount_;
	rightPos_.z += slideAmount_;

}

void Door::Draw(void)
{
	MV1SetPosition(leftHandle_, leftPos_);
	MV1DrawModel(leftHandle_);

	MV1SetPosition(rightHandle_, rightPos_);
	MV1DrawModel(rightHandle_);

	DrawFormatString(
		0, 400,
		0xffffff,
		"door : %.2f %.2f %.2f",
		rightPos_.x, rightPos_.y, rightPos_.z);

	DrawFormatString(
		0, 450,
		0xffffff,
		"door : %.2f %.2f %.2f",
		leftPos_.x, leftPos_.y, leftPos_.z);
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
	//モデルの読み込み
	leftHandle_ =
		resMng_.Load(
			ResourceManager::SRC::DOOR_LEFT).handleId_;

	rightHandle_ =
		resMng_.Load(
			ResourceManager::SRC::DOOR_RIGHT).handleId_;
}

void Door::InitTransform(void)
{
	leftBasePos_ = VGet(230.0f, 400.0f, 0.0f);
	rightBasePos_ = VGet(230.0f,400.0f, 0.0f);

	leftPos_ = leftBasePos_;
	rightPos_ = rightBasePos_;

	
	MV1SetScale(leftHandle_, VGet(0.3f, 0.3f, 0.3f));
	MV1SetScale(rightHandle_, VGet(0.3f, 0.3f, 0.3f));

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
