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
	//ƒ‚ƒfƒ‹‚Ì“Ç‚Ýž‚Ý
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

	MATRIX localRot = Quaternion::Euler(currentOffset_.rotEuler).ToMatrix();
	MATRIX localTrans = MGetTranslate(currentOffset_.localPos);
	MATRIX scaleMatrix = MGetScale(VGet(0.01f, 0.01f, 0.01f));

	int playerModelId = player_->GetTransform().modelId;
	int rightHandFrame = MV1SearchFrame(playerModelId, "hand.R");
	if (rightHandFrame == -1) return;

	MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, rightHandFrame);

	MATRIX scaleWithRot = MMult(scaleMatrix, localRot);
	MATRIX withHand = MMult(scaleWithRot, handMatrix);
	MATRIX finalMatrix = MMult(localTrans, withHand);

	MV1SetMatrix(transform_.modelId, finalMatrix);

	transform_.pos = VGet(
		finalMatrix.m[3][0],
		finalMatrix.m[3][1],
		finalMatrix.m[3][2]);

	transform_.Update();
}