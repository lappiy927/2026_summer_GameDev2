#include <DxLib.h>
#include "../../Utility/AsoUtility.h"
#include "Transform.h"

Transform::Transform(void)
	:
	modelId_(-1),
	scl_(AsoUtility::VECTOR_ONE),
	rot_(AsoUtility::VECTOR_ZERO),
	pos_(AsoUtility::VECTOR_ZERO),
	localPos_(AsoUtility::VECTOR_ZERO),
	matScl_(MGetIdent()),
	matRot_(MGetIdent()),
	matPos_(MGetIdent()),
	quaRot_(Quaternion::Identity()),
	quaRotLocal_(Quaternion::Identity())

{
}

Transform::~Transform(void)
{
}

void Transform::Update(void)
{
	// 大きさ
	matScl_ = MGetScale(scl_);

	// 回転
	rot_ = quaRot_.ToEuler();
	matRot_ = quaRot_.ToMatrix();

	// 位置
	matPos_ = MGetTranslate(pos_);

	// 行列の合成
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl_);
	Quaternion q = quaRot_.Mult(quaRotLocal_);
	mat = MMult(mat, q.ToMatrix());
	mat = MMult(mat, matPos_);

	// 行列をモデルに判定
	if (modelId_ != -1)
	{
		MV1SetMatrix(modelId_, mat);
	}
}

void Transform::Release(void)
{
}

void Transform::SetModel(int id)
{
	modelId_ = id;
}

VECTOR Transform::GetForward(void) const
{
	return GetDir(AsoUtility::DIR_F);
}

VECTOR Transform::GetBack(void) const
{
	return GetDir(AsoUtility::DIR_B);
}

VECTOR Transform::GetRight(void) const
{
	return GetDir(AsoUtility::DIR_R);
}

VECTOR Transform::GetLeft(void) const
{
	return GetDir(AsoUtility::DIR_L);
}

VECTOR Transform::GetUp(void) const
{
	return GetDir(AsoUtility::DIR_U);
}

VECTOR Transform::GetDown(void) const
{
	return GetDir(AsoUtility::DIR_D);
}

VECTOR Transform::GetDir(const VECTOR& dir) const
{
	return quaRot_.PosAxis(dir);
}

MATRIX Transform::GetWorldMatrix() const
{
	MATRIX mat = MGetIdent();
	mat = MMult(mat, matScl_);
	Quaternion q = quaRot_.Mult(quaRotLocal_);
	mat = MMult(mat, q.ToMatrix());
	mat = MMult(mat, matPos_);  // pos の行列化は matPos に入れる
	return mat;
}