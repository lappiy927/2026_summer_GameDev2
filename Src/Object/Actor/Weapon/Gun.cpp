#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../Common/Transform.h"
#include "../../Collider/ColliderCapsule.h"
#include "Gun.h"

int Gun::remainingBullets_ = Gun::MAX_BULLET_COUNT;

Gun::Gun(void)
	:
	WeaponBase()
{
}

Gun::~Gun(void)
{
	if (bulletCountFontHandle_ != -1)
	{
		DeleteFontToHandle(bulletCountFontHandle_);
	}
}

void Gun::Init(void)
{
	InitLoad();
	InitTransform();
	InitCollider();

	effectHandle = LoadEffekseerEffect(_T("Data/effect/flash.efk"), 5.0f);

	bulletCountHandle_ = LoadGraph("Data/Image/BulletCount.png");

	bulletCountFontHandle_ = CreateFontToHandle(
		NULL,
		BULLET_COUNT_FONT_SIZE,
		6,
		DX_FONTTYPE_ANTIALIASING_EDGE);
}

void Gun::Update(const Transform& playerTransform, WEAPON_STATE weaponState)
{
	auto currentAnim = weaponState;

	switch (currentAnim)
	{
	case WEAPON_STATE::IDLE:
		UpdateIdle();
		break;
	case WEAPON_STATE::MOVE:
		UpdateMove();
		break;
	case WEAPON_STATE::DASH:
		UpdateDash();
		break;
	case WEAPON_STATE::JUMP:
		UpdateJump();
		break;
	case WEAPON_STATE::ATTACK:
		UpdateAttack(playerTransform);
		break;
	case WEAPON_STATE::RELOAD:
		UpdateReload();
		break;
	}

	UpdateTransform(playerTransform);

	// 弾の更新
	UpdateBullet();

	prevWeaponState_ = currentAnim;
}

void Gun::Draw(void)
{
	WeaponBase::Draw();
	dynamic_cast<ColliderCapsule*>(attackCollider_)->DrawDebug(0xff0000);

	// 弾の描画
	if (bulletState_ == BULLET_STATE::FLYING)
	{
		DrawSphere3D(bulletPos_, BULLET_RADIUS, 8, 0xffff00, 0xffaa00, TRUE);
	}

	// 残弾数表示
	int bulletImgW = 0;
	int bulletImgH = 0;
	GetGraphSize(bulletCountHandle_, &bulletImgW, &bulletImgH);

	DrawGraph(30, 550, bulletCountHandle_, TRUE);

	DrawFormatStringToHandle(
		30 + bulletImgW * 3 / 4 - 20,
		550 + bulletImgH / 2 - (BULLET_COUNT_FONT_SIZE / 2),
		0xffffff,
		bulletCountFontHandle_,
		"%d/%d",
		remainingBullets_,
		MAX_BULLET_COUNT);
}

ColliderCapsule* Gun::GetCollider(void) const
{
	return dynamic_cast<ColliderCapsule*>(attackCollider_);
}

void Gun::InitLoad(void)
{
	transform_.SetModel(
		resMng_.Load(ResourceManager::SRC::GUN).handleId_);
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
		VGet(0, 0, 100),
		VGet(0, 0, 300),
		100.0f);
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
	currentOffset_ = OFFSET_DASH;
}

void Gun::UpdateJump(void)
{
	currentOffset_ = OFFSET_JUMP;
}

void Gun::UpdateAttack(const Transform& playerTransform)
{
	// 入った瞬間だけ発射
	if (prevWeaponState_ != WEAPON_STATE::ATTACK)
	{
		if (bulletState_ == BULLET_STATE::IDLE && remainingBullets_ > 0)
		{
			FireBullet(playerTransform);
		}
	}

	currentOffset_ = OFFSET_ATTACK;
}

void Gun::UpdateReload(void)
{
	currentOffset_ = OFFSET_IDLE;

	if (prevWeaponState_ != WEAPON_STATE::RELOAD)
	{
		bulletState_ = BULLET_STATE::IDLE;
		attackCollider_->SetEnable(false);
	}
}

void Gun::UpdateTransform(const Transform& playerTransform)
{
	int playerModelId = playerTransform.modelId;
	int rightHandFrame = MV1SearchFrame(playerModelId, "hand.L");
	if (rightHandFrame == -1) return;

	MATRIX handMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, rightHandFrame);

	VECTOR handPos = VGet(
		handMatrix.m[3][0],
		handMatrix.m[3][1],
		handMatrix.m[3][2]);

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

	VECTOR rotatedTop = VTransform(VGet(0.0f, 0.0f, 0.0f), rotMatrix);
	VECTOR rotatedDown = VTransform(VGet(0.0f, 150.0f, 0.0f), rotMatrix);

	VECTOR offset = VTransform(currentOffset_.localPos, rotMatrix);
	VECTOR finalPos = VAdd(handPos, offset);

	MATRIX finalMatrix = rotMatrix;
	finalMatrix.m[3][0] = finalPos.x;
	finalMatrix.m[3][1] = finalPos.y;
	finalMatrix.m[3][2] = finalPos.z;

	MV1SetMatrix(transform_.modelId, finalMatrix);

	attackCollider_->SetLocalPosTop(VAdd(rotatedTop, finalPos));
	attackCollider_->SetLocalPosDown(VAdd(rotatedDown, finalPos));
}



void Gun::FireBullet(const Transform& playerTransform)
{
	int playerModelId = playerTransform.modelId;

	int hipFrame = MV1SearchFrame(playerModelId, "spine");
	if (hipFrame == -1) return;

	MATRIX hipMatrix = MV1GetFrameLocalWorldMatrix(playerModelId, hipFrame);

	VECTOR rayOrigin = VGet(
		hipMatrix.m[3][0],
		hipMatrix.m[3][1],
		hipMatrix.m[3][2]);

	static constexpr float MUZZLE_FORWARD_OFFSET = 50.0f;
	float yaw = playerTransform.quaRot.ToEuler().y;
	VECTOR forward = VGet(sinf(yaw), 0.0f, cosf(yaw));

	bulletOrigin_ = VAdd(rayOrigin, VScale(forward, MUZZLE_FORWARD_OFFSET));
	bulletPos_ = bulletOrigin_;
	bulletDir_ = forward;
	bulletState_ = BULLET_STATE::FLYING;

	// 残弾数を1消費する(ゲームシーン・ボスシーンで共有)
	remainingBullets_--;

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
		// 弾を前進させる
		bulletPos_ = VAdd(bulletPos_, VScale(bulletDir_, BULLET_SPEED));

		// 当たり判定を弾の位置に合わせる
		attackCollider_->SetLocalPosTop(bulletPos_);
		attackCollider_->SetLocalPosDown(bulletPos_);
		attackCollider_->SetEnable(true);

		// 射程を超えたらリロードへ
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
