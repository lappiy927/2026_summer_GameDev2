#include "../Common/Transform.h"
#include "ColliderBase.h"

ColliderBase::ColliderBase(SHAPE shape, TAG tag, const Transform* follow)
	:
	shape_(shape),
	tag_(tag),
	follow_(follow),
	isValid_(true)
{
}

ColliderBase::~ColliderBase(void)
{
}

void ColliderBase::Draw(void)
{
	int color = COLOR_INVALID;

	if (isValid_)
	{
		color = COLOR_VALID;
	}
	DrawDebug(color);
}

void ColliderBase::SetFollow(const Transform* follow)
{
	follow_ = follow;
}

void ColliderBase::SetEnable(bool enable)
{
	isEnable_ = enable;
}

bool ColliderBase::IsEnable() const
{
	return isEnable_;
}

VECTOR ColliderBase::GetRotPos(const VECTOR& localPos) const
{
	// 追従相手の回転に合わせて指定ローカル座標を回転し、
	// 基準座標に加えることでワールド座標へ変換
	VECTOR localRotPos = follow_->quaRot_.PosAxis(localPos);
	return VAdd(follow_->pos_, localRotPos);
}