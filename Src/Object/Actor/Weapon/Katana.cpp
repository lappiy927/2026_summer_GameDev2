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
	//モデルの読み込み
	transform_.SetModel(
		resMng_.Load(
			ResourceManager::SRC::KATANA).handleId_);

	attackCollider_ = new ColliderCapsule(
		ColliderBase::TAG::PLAYER_ATTACK,
		&transform_,
		VGet(0, 0, 100),
		VGet(0, 0, 300),
		150.0f);
}

void Katana::Update(void)
{

	// プレイヤーのモデルIDを取得
	int playerModelId = player_->GetTransform().modelId;

	// プレイヤーの右手のフレームIDを取得
	int rightHandFrame = MV1SearchFrame(playerModelId, "hand.R");


	if (rightHandFrame != -1) {

		// 刀の位置を手に追従させる
		MATRIX handMatrix =
			MV1GetFrameLocalWorldMatrix(playerModelId, rightHandFrame);

		// 位置取得
		VECTOR handPos = VGet(
			handMatrix.m[3][0],
			handMatrix.m[3][1],
			handMatrix.m[3][2]
		);

		transform_.pos = handPos;

		//transform_.Update();

		// 回転用行列作成
		MATRIX rotMatrix = handMatrix;

		// 手の位置に追従
		MV1SetPosition(transform_.modelId, handPos);

		// 手の回転に追従
		MV1SetRotationMatrix(transform_.modelId, rotMatrix);

		// サイズ固定
		MV1SetScale(transform_.modelId, VGet(0.01f, 0.01f, 0.01f));
	}

	bool mouse =
		(GetMouseInput() & MOUSE_INPUT_LEFT);

	if (mouse && !oldMouse_)
	{
		isAttack_ = true;

		attackTimer_ = 0.2f;
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
