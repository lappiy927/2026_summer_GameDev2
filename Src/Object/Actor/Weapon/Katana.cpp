#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Object/Actor/Charactor/Player.h"
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

		// 回転用行列作成
		MATRIX rotMatrix = handMatrix;

		// 手の位置に追従
		MV1SetPosition(transform_.modelId, handPos);

		// 手の回転に追従
		MV1SetRotationMatrix(transform_.modelId, rotMatrix);

		// サイズ固定
		MV1SetScale(transform_.modelId, VGet(0.01f, 0.01f, 0.01f));
	}
}