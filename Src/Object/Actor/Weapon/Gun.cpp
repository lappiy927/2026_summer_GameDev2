#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Object/Actor/Charactor/Player.h"
#include "../../Collider/ColliderCapsule.h"
#include "Gun.h"

Gun::Gun(Player* player)
	:
	WeaponBase(player)
{
}

Gun::~Gun(void)
{
}

void Gun::Init(void)
{
	InitLoad();
	InitTransform();
	InitCollider();
	effectHandle = LoadEffekseerEffect(_T("Data/effect/flash.efk"), 5.0f);
}

void Gun::Update(void)
{
	auto currentAnim = player_->GetAnimType();

	// ATTACKèÛë‘Ç…ì¸Ç¡ÇΩèuä‘Ç…î≠éÀ
	if (currentAnim == Player::ANIM_TYPE::ATTACK &&
		prevAnimType_ != Player::ANIM_TYPE::ATTACK)
	{
		if (bulletState_ == BULLET_STATE::IDLE)
		{
			FireBullet();
		}
	}


	switch (currentAnim)
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
	case Player::ANIM_TYPE::JUMP:
		UpdateJump();
		break;
	case Player::ANIM_TYPE::RELOAD:
		UpdateReload();
		break;
	}

	// íeÇÃçXêV
	UpdateBullet();

	UpdateTransform();

	prevAnimType_ = currentAnim;
}

void Gun::Draw(void)
{
	WeaponBase::Draw();
	dynamic_cast<ColliderCapsule*>(attackCollider_)->DrawDebug(0xff0000);

	// íeÇÃï`âÊ
	if (bulletState_ == BULLET_STATE::FLYING)
	{
		DrawSphere3D(bulletPos_, BULLET_RADIUS, 8, 0xffff00, 0xffaa00, TRUE);
	}
}

ColliderCapsule* Gun::GetCollider() const
{
	return attackCollider_;
}

void Gun::InitLoad(void)
{
	transform_.SetModel(
		resMng_.Load(
			ResourceManager::SRC::GUN).handleId_);
}

void Gun::InitTransform(void)
{
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.pos = VGet(0.0f, 0.0f, 0.0f);
	transform_.Update();
}

void Gun::InitCollider(void)
{
	attackCollider_ = new ColliderCapsule(
		ColliderBase::TAG::PLAYER_ATTACK,
		&transform_,
		VGet(0, 0, 0),
		VGet(0, 0, 0),
		BULLET_RADIUS);
	attackCollider_->SetEnable(false);
}

void Gun::UpdateIdle(void)
{
	currentOffset_ = OFFSET_IDLE;
}

void Gun::UpdateMove(void)
{
	currentOffset_ = OFFSET_MOVE;
}

void Gun::UpdateDash(void)
{
	currentOffset_ = OFFSET_MOVE;
}

void Gun::UpdateAttack(void)
{
	// ì¸Ç¡ÇΩèuä‘ÇæÇØî≠éÀ
	if (prevAnimType_ != Player::ANIM_TYPE::ATTACK)
	{
		if (bulletState_ == BULLET_STATE::IDLE)
		{
			FireBullet();
		}
	}
}

void Gun::UpdateJump(void)
{
	currentOffset_ = OFFSET_JUMP;
}

void Gun::UpdateReload(void)
{
	currentOffset_ = OFFSET_IDLE;

	if (prevAnimType_ != Player::ANIM_TYPE::RELOAD)
	{
		bulletState_ = BULLET_STATE::IDLE;
		attackCollider_->SetEnable(false);
	}
}

void Gun::FireBullet(void)
{
	int playerModelId = player_->GetTransform().modelId;

	int hipFrame = MV1SearchFrame(playerModelId, "spine");
	if (hipFrame == -1) return;

	MATRIX hipMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, hipFrame);

	VECTOR rayOrigin = VGet(
		hipMatrix.m[3][0],
		hipMatrix.m[3][1],
		hipMatrix.m[3][2]);

	static constexpr float MUZZLE_FORWARD_OFFSET = 50.0f;
	float yaw = player_->GetTransform().quaRot.ToEuler().y;
	VECTOR forward = VGet(sinf(yaw), 0.0f, cosf(yaw));

	bulletOrigin_ = VAdd(rayOrigin, VScale(forward, MUZZLE_FORWARD_OFFSET));
	bulletPos_ = bulletOrigin_;
	bulletDir_ = forward;
	bulletState_ = BULLET_STATE::FLYING;

	int playHandle = PlayEffekseer3DEffect(effectHandle);
	SetPosPlayingEffekseer3DEffect(playHandle, bulletOrigin_.x, bulletOrigin_.y, bulletOrigin_.z);
	SetRotationPlayingEffekseer3DEffect(playHandle, 0.0f, yaw, 0.0f);
}

void Gun::UpdateBullet(void)
{
	float deltaTime = SceneManager::GetInstance().GetDeltaTime();

	switch (bulletState_)
	{
	case BULLET_STATE::FLYING:
	{
		// íeÇëOêiÇ≥ÇπÇÈ
		bulletPos_ = VAdd(bulletPos_, VScale(bulletDir_, BULLET_SPEED));

		// ìñÇΩÇËîªíËÇíeÇÃà íuÇ…çáÇÌÇπÇÈ
		attackCollider_->SetLocalPosTop(bulletPos_);
		attackCollider_->SetLocalPosDown(bulletPos_);
		attackCollider_->SetEnable(true);

		// éÀíˆÇí¥Ç¶ÇΩÇÁÉäÉçÅ[ÉhÇ÷
		VECTOR diff = VSub(bulletPos_, bulletOrigin_);
		float distSq = VDot(diff, diff);
		if (distSq >= BULLET_MAX_RANGE * BULLET_MAX_RANGE)
		{
			bulletState_ = BULLET_STATE::RELOADING;
			attackCollider_->SetEnable(false);
		}
		break;
	}

	case BULLET_STATE::RELOADING:
	{
		break;
	}

	case BULLET_STATE::IDLE:
	default:
		attackCollider_->SetEnable(false);
		break;
	}
}

void Gun::UpdateTransform(void)
{
	int playerModelId = player_->GetTransform().modelId;

	int rightHandFrame = MV1SearchFrame(playerModelId, "hand.L");
	if (rightHandFrame == -1) return;

	MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, rightHandFrame);

	MATRIX handRotOnly = MGetIdent();
	VECTOR col0 = VNorm(VGet(handMatrix.m[0][0], handMatrix.m[0][1], handMatrix.m[0][2]));
	VECTOR col1 = VNorm(VGet(handMatrix.m[1][0], handMatrix.m[1][1], handMatrix.m[1][2]));
	VECTOR col2 = VNorm(VGet(handMatrix.m[2][0], handMatrix.m[2][1], handMatrix.m[2][2]));
	handRotOnly.m[0][0] = col0.x; handRotOnly.m[0][1] = col0.y; handRotOnly.m[0][2] = col0.z;
	handRotOnly.m[1][0] = col1.x; handRotOnly.m[1][1] = col1.y; handRotOnly.m[1][2] = col1.z;
	handRotOnly.m[2][0] = col2.x; handRotOnly.m[2][1] = col2.y; handRotOnly.m[2][2] = col2.z;
	handRotOnly.m[3][0] = 0.0f;   handRotOnly.m[3][1] = 0.0f;   handRotOnly.m[3][2] = 0.0f;

	MATRIX offsetRot = Quaternion::Euler(currentOffset_.rotEuler).ToMatrix();
	MATRIX rotMatrix = MMult(offsetRot, handRotOnly);

	VECTOR offset = VTransform(currentOffset_.localPos, rotMatrix);
	VECTOR finalPos = VAdd(VGet(handMatrix.m[3][0], handMatrix.m[3][1], handMatrix.m[3][2]), offset);

	MATRIX finalMatrix = rotMatrix;
	finalMatrix.m[3][0] = finalPos.x;
	finalMatrix.m[3][1] = finalPos.y;
	finalMatrix.m[3][2] = finalPos.z;

	MV1SetMatrix(transform_.modelId, finalMatrix);
}