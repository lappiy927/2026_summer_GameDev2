#include "../../../Application.h"
#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/ResourceManager.h"
#include "../../Common/AnimationController.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderModel.h"
#include "../../Collider/ColliderCapsule.h"
#include "CharactorBase.h"

CharactorBase::CharactorBase(void)
	:
	ActorBase(),
	animationController_(nullptr),
	moveDir_(AsoUtility::VECTOR_ZERO),
	moveSpeed_(0.0f),
	movePow_(AsoUtility::VECTOR_ZERO),
	jumpPow_(AsoUtility::VECTOR_ZERO),
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

	//アニメーション再生
	animationController_->Update();

	//各キャラクターごとの更新後処理
	UpdateProcessPost();

	//モデル制御更新
	transform_.Update();

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
	// ゼロベクトルなら回転しない
	if (VSize(moveDir_) <= 0.001f)
	{
		return;
	}


	//移動方向から回転に変換する
	Quaternion goalRot =
		Quaternion::LookRotation(moveDir_);

	transform_.quaRot =
		Quaternion::Slerp(
			transform_.quaRot,
			goalRot,
			0.2f);

	// モデル補正
	transform_.quaRotLocal =
		Quaternion::AngleAxis(
			DX_PI_F,
			AsoUtility::DIR_U);
}

void CharactorBase::CalcGravityPow(void)
{
	//重力方向
	VECTOR dirGravity = AsoUtility::DIR_D;

	//重力の強さ
	float gravityPow =
		Application::GetInstance().GetGravityPow()
		* scnMng_.GetDeltaTime();

	//重力
	VECTOR gravity = VScale(dirGravity, gravityPow);
	jumpPow_ = VAdd(jumpPow_, gravity);

}

void CharactorBase::Collision(void)
{

	// 移動処理
	transform_.pos = VAdd(transform_.pos, movePow_);

	// ジャンプ量を加算
	transform_.pos = VAdd(transform_.pos, jumpPow_);

	// 地面との衝突
	CollisionGravity();

	// 壁との衝突
	CollisionCapsule();
}

void CharactorBase::CollisionGravity(void)
{


	isSteepSlope_ = false;

	int lineType = static_cast<int>(COLLIDER_TYPE::LINE);
	if (ownColliders_.count(lineType) == 0) return;

	ColliderLine* colliderLine_ =
		dynamic_cast<ColliderLine*>(ownColliders_.at(lineType));
	if (colliderLine_ == nullptr) return;

	VECTOR s = colliderLine_->GetPosStart();
	VECTOR e = colliderLine_->GetPosEnd();

	bool isHitGround = false;
	float nearestY = -FLT_MAX;
	float nearestNormalY = 1.0f;

	// 始点(上)に最も近い衝突点を選ぶ
	float minDistFromStart = FLT_MAX;

	for (const auto& hitCol : hitColliders_)
	{
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);
		if (colliderModel == nullptr) continue;

		auto hits = MV1CollCheck_LineDim(
			colliderModel->GetFollow()->modelId, -1, s, e);

		for (int i = 0; i < hits.HitNum; i++)
		{
			auto hit = hits.Dim[i];

			if (colliderModel->IsExcludeFrame(hit.FrameIndex))
				continue;

			// 始点(足元上)から衝突点までの距離で最近点を選ぶ
			float dist = fabs(s.y - hit.HitPosition.y);
			if (dist < minDistFromStart)
			{
				minDistFromStart = dist;
				nearestY = hit.HitPosition.y;
				nearestNormalY = hit.Normal.y;
				isHitGround = true;
			}

			float slopeLimit = 0.75f;

			if (hit.Normal.y >= slopeLimit)
			{
				transform_.pos.y = hit.HitPosition.y + 10.0f;

				jumpPow_ = AsoUtility::VECTOR_ZERO;

				// ジャンプ判定
				isJump_ = false;
			}
			else
			{
				if (!isPlayer_)
				{
					VECTOR slideDir =
					{
						hit.Normal.x,
						0.0f,
						hit.Normal.z
					};

					if (VSize(slideDir) > 0.001f)
					{
						slideDir = VNorm(slideDir);

						float slidePower =
							(1.0f - hit.Normal.y) * 5.0f;

						transform_.pos =
							VAdd(
								transform_.pos,
								VScale(slideDir, slidePower)
							);
					}
				}

				char str[256];
				sprintf_s(str,
					"SteepSlope! Normal=(%.2f, %.2f, %.2f)\n",
					hit.Normal.x,
					hit.Normal.y,
					hit.Normal.z);
				OutputDebugStringA(str);

				isSteepSlope_ = true;

				// 登れなくする
				movePow_.x = 0.0f;
				movePow_.z = 0.0f;
			}
<<<<<<< HEAD

		

=======
>>>>>>> e0a521df7ff0644fe40073394f28e038b1a05487
		}

		MV1CollResultPolyDimTerminate(hits);
	}

	if (isHitGround)
	{
		float slopeLimit = 0.75f;

		// 地面より下に来たら着地させる
		if (transform_.pos.y < nearestY + 2.0f)
		{
			transform_.pos.y = nearestY + 2.0f;
			jumpPow_ = AsoUtility::VECTOR_ZERO;
			isJump_ = false;
		}

		if (nearestNormalY >= slopeLimit)
		{
			if (!isJump_)
			{
				transform_.pos.y = nearestY + 10.0f;
			}
		}
		else
		{
			isSteepSlope_ = true;
			movePow_.x = 0.0f;
			movePow_.z = 0.0f;

			if (!isJump_)
			{
				transform_.pos.y = nearestY + 10.0f;
			}
		}
	}

	if (!isJump_)
	{
		jumpPow_ = AsoUtility::VECTOR_ZERO;
		stepJump_ = 0.0f;
	}
}

void CharactorBase::CollisionCapsule(void)
{
	/*if (isSteepSlope_)
	{
		return;
	}*/

	if (isSteepSlope_)
	{
		OutputDebugStringA("SteepSlope ON\n");
	}


	// カプセルコライダ
	int capsuleType = static_cast<int>(COLLIDER_TYPE::CAPSULE);

	// カプセルコライダが無ければ処理を抜ける
	if (ownColliders_.count(capsuleType) == 0) return;

	// カプセルコライダ情報
	ColliderCapsule* colliderCapsule =
		dynamic_cast<ColliderCapsule*>(ownColliders_.at(capsuleType));

	if (colliderCapsule == nullptr) return;

	// 登録されている衝突物を全てチェック
	for (const auto& hitCol : hitColliders_)
	{
		// モデル以外は処理を飛ばす
		if (hitCol->GetShape() != ColliderBase::SHAPE::MODEL) continue;

		// 派生クラスへキャスト
		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);

		if (colliderModel == nullptr) continue;

		//指定された回数と距離で三角形の法線方向に押し戻す
		colliderCapsule->PushBackAlongNormal(
			colliderModel, transform_,
			CNT_TRY_COLLISION, COLLISION_BACK_DIS,
			true, false
		);
	}
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

	// 登録されているステージコリジョンすべてチェック
	for (auto& hitCol : hitColliders_)
	{
		// ステージ以外は対象外
		if (hitCol->GetTag() != ColliderBase::TAG::STAGE) continue;

		const ColliderModel* colliderModel =
			dynamic_cast<const ColliderModel*>(hitCol);
		if (!colliderModel) continue;

		int modelHandle = colliderModel->GetFollow()->modelId;

		// カプセルの下方向へ影を落とすポリゴンを取得
		MV1_COLL_RESULT_POLY_DIM hitDim =
			MV1CollCheck_Capsule(
				modelHandle, -1,
				playerPos,
				VAdd(playerPos, VGet(0, -PLAYER_SHADOW_HEIGHT, 0)),
				PLAYER_SHADOW_SIZE
			);

		// 頂点データ共通設定
		VERTEX3D v[3];
		v[0].dif = GetColorU8(255, 255, 255, 255);
		v[0].spc = GetColorU8(0, 0, 0, 0);
		v[0].su = 0.0f;  v[0].sv = 0.0f;
		v[1] = v[0];
		v[2] = v[0];

		MV1_COLL_RESULT_POLY* hit = hitDim.Dim;
		for (int i = 0; i < hitDim.HitNum; i++, hit++)
		{
			// 除外フレームは無視
			if (colliderModel->IsExcludeFrame(hit->FrameIndex)) continue;

			// ポリゴン頂点をセット
			v[0].pos = hit->Position[0];
			v[1].pos = hit->Position[1];
			v[2].pos = hit->Position[2];

			// 浮かせて Z-fighting を防止
			VECTOR slideVec = VScale(hit->Normal, 0.5f);
			v[0].pos = VAdd(v[0].pos, slideVec);
			v[1].pos = VAdd(v[1].pos, slideVec);
			v[2].pos = VAdd(v[2].pos, slideVec);

			// 距離によって α値調整（自然な影の薄さ）
			v[0].dif.a =
				(hit->Position[0].y > playerPos.y - PLAYER_SHADOW_HEIGHT)
				? (BYTE)(128 * (1.0f - fabs(hit->Position[0].y - playerPos.y) / PLAYER_SHADOW_HEIGHT))
				: 0;

			v[1].dif.a =
				(hit->Position[1].y > playerPos.y - PLAYER_SHADOW_HEIGHT)
				? (BYTE)(128 * (1.0f - fabs(hit->Position[1].y - playerPos.y) / PLAYER_SHADOW_HEIGHT))
				: 0;

			v[2].dif.a =
				(hit->Position[2].y > playerPos.y - PLAYER_SHADOW_HEIGHT)
				? (BYTE)(128 * (1.0f - fabs(hit->Position[2].y - playerPos.y) / PLAYER_SHADOW_HEIGHT))
				: 0;

			// UV をプレイヤー中心からの相対位置で算出
			v[0].u = (hit->Position[0].x - playerPos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			v[0].v = (hit->Position[0].z - playerPos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			v[1].u = (hit->Position[1].x - playerPos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			v[1].v = (hit->Position[1].z - playerPos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			v[2].u = (hit->Position[2].x - playerPos.x) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;
			v[2].v = (hit->Position[2].z - playerPos.z) / (PLAYER_SHADOW_SIZE * 2.0f) + 0.5f;

			// 影ポリゴンを描画
			DrawPolygon3D(v, 1, imgShadow_, TRUE);
		}

		// メモリ解放
		MV1CollResultPolyDimTerminate(hitDim);
	}

	// ライティングON
	SetUseLighting(TRUE);

	// Zバッファを無効に戻す
	SetUseZBuffer3D(FALSE);
}

