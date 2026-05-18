#include <DxLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Object/Actor/Charactor/Player.h"
#include "../../Collider/ColliderCapsule.h"

#include "Katana.h"

Katana::Katana(Player* player)
	:
	WeaponBase(player)
{
}

Katana::~Katana(void)
{
}

void Katana::Init(void)
{
	InitLoad();

	InitTransform();

	InitCollider();
}

void Katana::Update(void)
{

	switch (player_->GetAnimType())
	{
	case Player::ANIM_TYPE::IDLE:
		UpdateIdle();
		break;
	case Player::ANIM_TYPE::RUN:
		UpdateMove();
		break;
	case Player::ANIM_TYPE::FAST_RUN:
		UpdateDash();
		break;
	case Player::ANIM_TYPE::ATTACK:
		UpdateAttack();
		break;
	}

	UpdateTransform();

	bool mouse =
		(GetMouseInput() & MOUSE_INPUT_LEFT);

	if (mouse && !oldMouse_)
	{
		isAttack_ = true;

		attackTimer_ = 0.2f;

		player_->PlayAttackAnimation();
	}

	oldMouse_ = mouse;
	
	if (isAttack_)
	{
		attackTimer_ -= scnMng_.GetDeltaTime();

		if (attackTimer_ <= 0.0f)
		{
			isAttack_ = false;
		}
	}

	attackCollider_->SetEnable(isAttack_);
	dynamic_cast<ColliderCapsule*>(attackCollider_)->DrawDebug(0xff0000);


}


ColliderCapsule* Katana::GetCollider() const
{
	return attackCollider_;
}

void Katana::InitLoad(void)
{
	//モデルの読み込み
	transform_.SetModel(
		resMng_.Load(
			ResourceManager::SRC::KATANA).handleId_);
}

void Katana::InitTransform(void)
{
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.pos = VGet(0.0f, 0.0f, 0.0f);
	transform_.Update();
}

void Katana::InitCollider(void)
{
	attackCollider_ = new ColliderCapsule(
		ColliderBase::TAG::PLAYER_ATTACK,
		&transform_,
		VGet(0, 0, 100),
		VGet(0, 0, 300),
		150.0f);
}

void Katana::UpdateIdle(void)
{
	currentOffset_ = OFFSET_IDLE;
}

void Katana::UpdateMove(void)
{
	currentOffset_ = OFFSET_MOVE;
}

void Katana::UpdateDash(void)
{
	currentOffset_ = OFFSET_MOVE;
}

void Katana::UpdateAttack(void)
{
	currentOffset_ = OFFSET_ATTACK;
}

void Katana::UpdateTransform(void)
{
	int playerModelId = player_->GetTransform().modelId;
	int rightHandFrame = MV1SearchFrame(playerModelId, "hand.R");
	if (rightHandFrame == -1) return;

	MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, rightHandFrame);

	VECTOR handPos = VGet(
		handMatrix.m[3][0],
		handMatrix.m[3][1],
		handMatrix.m[3][2]);

	// 行列の各列を正規化してスケールを除去
	MATRIX handRotOnly = MGetIdent();
	VECTOR col0 = VNorm(VGet(handMatrix.m[0][0], handMatrix.m[0][1], handMatrix.m[0][2]));
	VECTOR col1 = VNorm(VGet(handMatrix.m[1][0], handMatrix.m[1][1], handMatrix.m[1][2]));
	VECTOR col2 = VNorm(VGet(handMatrix.m[2][0], handMatrix.m[2][1], handMatrix.m[2][2]));
	handRotOnly.m[0][0] = col0.x; handRotOnly.m[0][1] = col0.y; handRotOnly.m[0][2] = col0.z;
	handRotOnly.m[1][0] = col1.x; handRotOnly.m[1][1] = col1.y; handRotOnly.m[1][2] = col1.z;
	handRotOnly.m[2][0] = col2.x; handRotOnly.m[2][1] = col2.y; handRotOnly.m[2][2] = col2.z;
	handRotOnly.m[3][0] = 0.0f;   handRotOnly.m[3][1] = 0.0f;   handRotOnly.m[3][2] = 0.0f;

	MATRIX offsetRot = Quaternion::Euler(currentOffset_.rotEuler).ToMatrix();

	// ★ 回転のみ合成（スケールは後で独立して適用）
	MATRIX rotMatrix = MMult(offsetRot, handRotOnly);

	// 最終行列（回転＋位置）
	MATRIX finalMatrix = rotMatrix;
	finalMatrix.m[3][0] = handPos.x;
	finalMatrix.m[3][1] = handPos.y;
	finalMatrix.m[3][2] = handPos.z;

	MV1SetMatrix(transform_.modelId, finalMatrix);

<<<<<<< .merge_file_6NJ73n
	transform_.pos = VGet(
		finalMatrix.m[3][0],
		finalMatrix.m[3][1],
		finalMatrix.m[3][2]);

	transform_.Update();
=======
	// 端点もrotMatrixで変換（刀モデルと同じ行列）
	VECTOR rotatedTop = VTransform(VGet(0.0f, -100.0f, 0.0f), rotMatrix);
	VECTOR rotatedDown = VTransform(VGet(0.0f, -300.0f, 0.0f), rotMatrix);

	// 位置オフセットもrotMatrixで変換
	VECTOR offset = VTransform(currentOffset_.localPos, rotMatrix);

	transform_.pos = handPos;
	transform_.quaRot = Quaternion::Identity();

	attackCollider_->SetLocalPosTop(VAdd(rotatedTop, offset));
	attackCollider_->SetLocalPosDown(VAdd(rotatedDown, offset));
>>>>>>> .merge_file_AYZJzW
}