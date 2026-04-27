#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/AnimationController.h"
#include "CharactorBase.h"

CharactorBase::CharactorBase(void)
	:
	ActorBase(),
	animationController_(nullptr),
	moveDir_(AsoUtility::VECTOR_ZERO),
	moveSpeed_(0.0f),
	movePow_(AsoUtility::VECTOR_ZERO),
	isJump_(false),
	imgShadow_(-1),
	stepJump_(-1)
{
}

CharactorBase::~CharactorBase(void)
{
}

void CharactorBase::Update(void)
{
	// 移動前座標を更新
	prevPos_ = transform_.pos;

	//各キャラクターごとの更新処理
	UpdateProcess();

	//移動方向に応じ遅延回転
	DelayRotate();

	//重力による移動量
	CalcGravityPow();

	// 衝突判定前準備
	CollisionReserve();

	// 衝突判定
	Collision();

	//モデル制御更新
	transform_.Update();

	//アニメーション再生
	animationController_->Update();

	//各キャラクターごとの更新後処理
	UpdateProcessPost();

}

void CharactorBase::Draw(void)
{
	// 基底クラスの描画処理
	ActorBase::Draw();

	// 丸影の描画
	DrawShadow();
}

void CharactorBase::Release(void)
{
	//アニメーションコントローラーの開放
	if (animationController_ != nullptr)
	{
		animationController_->Release();
		delete animationController_;
	}

	//基底クラス開放
	ActorBase::Release();
}

void CharactorBase::InitLoad(void)
{
	// 丸影画像
	imgShadow_ = resMng_.Load(ResourceManager::SRC::PLAYER_SHADOW).handleId_;
}

void CharactorBase::DelayRotate(void)
{
	//移動方向から回転に変換する
	Quaternion goalRot = Quaternion::LookRotation(moveDir_);

	//回転の補間
	transform_.quaRot =
		Quaternion::Slerp(transform_.quaRot, goalRot, 0.2f);
}

void CharactorBase::CalcGravityPow(void)
{
	//重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	//重力の強さ
	/*float gravityPow =
		Application::GetInstance().GetGravityPow()
		* scnMng_.GetDeltaTime();*/

	//重力
	//VECTOR gravity = VScale(dirGravity, gravityPow);
	//jumpPow_ = VAdd(jumpPow_, gravity);

}

void CharactorBase::Collision(void)
{
	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	// ジャンプ量を加算
	transform_.pos = VAdd(transform_.pos, jumpPow_);

}

void CharactorBase::DrawShadow(void)
{
	if (imgShadow_ < 0) return;

	const float PLAYER_SHADOW_HEIGHT = 800.0f;
	const float PLAYER_SHADOW_SIZE = 30.0f;

	VECTOR playerPos = transform_.pos;

	// ライティング無効（影は照明を受けない）
	SetUseLighting(FALSE);

	// Zバッファ有効
	SetUseZBuffer3D(TRUE);

	// テクスチャアドレスモードを CLAMP に（影の端が伸びるのを防ぐ）
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	//// 登録されているステージコリジョンすべてチェック
	//for (auto& hitCol : hitColliders_)
	//{
	//	// ステージ以外は対象外
	//	if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

	//	const ColliderModel* colliderModel =
	//		dynamic_cast<const ColliderModel*>(hitCol);
	//	if (!colliderModel) continue;

	//	int modelHandle = colliderModel->GetFollow()->modelId;

	//	// カプセルの下方向へ影を落とすポリゴンを取得
	//	MV1_COLL_RESULT_POLY_DIM hitDim =
	//		MV1CollCheck_Capsule(
	//			modelHandle, -1,
	//			playerPos,
	//			VAdd(playerPos, VGet(0, -PLAYER_SHADOW_HEIGHT, 0)),
	//			PLAYER_SHADOW_SIZE
	//		);

	//	// 頂点データ共通設定
	//	VERTEX3D v[3];
	//	v[0].dif = GetColorU8(255, 255, 255, 255);
	//	v[0].spc = GetColorU8(0, 0, 0, 0);
	//	v[0].su = 0.0f;  v[0].sv = 0.0f;
	//	v[1] = v[0];
	//	v[2] = v[0];

	//	MV1_COLL_RESULT_POLY* hit = hitDim.Dim;
	//	for (int i = 0; i < hitDim.HitNum; i++, hit++)
	//	{
	//		// 除外フレームは無視
	//		if (colliderModel->IsExcludeFrame(hit->FrameIndex)) continue;

	//		// ポリゴン頂点をセット
	//		v[0].pos = hit->Position[0];
	//		v[1].pos = hit->Position[1];
	//		v[2].pos = hit->Position[2];

	//		// 浮かせて Z-fighting を防止
	//		VECTOR slideVec = VScale(hit->Normal, 0.5f);
	//		v[0].pos = VAdd(v[0].pos, slideVec);
	//		v[1].pos = VAdd(v[1].pos, slideVec);
	//		v[2].pos = VAdd(v[2].pos, slideVec);

	//		// 距離によって α値調整（自然な影の薄さ）
	//		v[0].dif.a =
	//			(hit->Position[0].y > playerPos.y - PLAYER_SHADOW_HEIGHT)
	//			? (BYTE)(128 * (1.0f - fabs(hit->Position[0].y - playerPos.y) / PLAYER_SHADOW_HEIGHT))
	//			: 0;

	//		v[1].dif.a =
	//			(hit->Position[1].y > playerPos.y - PLAYER_SHADOW_HEIGHT)
	//			? (BYTE)(128 * (1.0f - fabs(hit->Position[1].y - playerPos.y) / PLAYER_SHADOW_HEIGHT))
	//			: 0;

	//		v[2].dif.a =
	//			(hit->Position[2].y > playerPos.y - PLAYER_SHADOW_HEIGHT)
	//			? (BYTE)(128 * (1.0f - fabs(hit->Position[2].y - playerPos.y) / PLAYER_SHADOW_HEIGHT))
	//			: 0;

	//		// UV をプレイヤー中心からの相対位置で算出
	//		v[0].u = (hit->Position[0].x - playerPos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
	//		v[0].v = (hit->Position[0].z - playerPos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

	//		v[1].u = (hit->Position[1].x - playerPos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
	//		v[1].v = (hit->Position[1].z - playerPos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

	//		v[2].u = (hit->Position[2].x - playerPos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
	//		v[2].v = (hit->Position[2].z - playerPos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

	//		// 影ポリゴンを描画
	//		DrawPolygon3D(v, 1, imgShadow_, TRUE);
	//	}

	//	// メモリ解放
	//	MV1CollResultPolyDimTerminate(hitDim);
	//}

	// ライティングON
	SetUseLighting(TRUE);

	// Zバッファを無効に戻す
	SetUseZBuffer3D(FALSE);
}

