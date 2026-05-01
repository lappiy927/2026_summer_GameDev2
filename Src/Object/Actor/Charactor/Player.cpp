#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/Camera.h"
#include "../../Common/AnimationController.h"
#include "../../../Object/Collider/ColliderLine.h"
#include "../../../Object/Collider/ColliderCapsule.h"
#include "../../../Application.h"
#include "Player.h"

Player::Player(void)
	:
	CharactorBase()
{
}

Player::~Player(void)
{
}

void Player::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	//モデルの読み込み
	transform_.SetModel(
		resMng_.Load(
			ResourceManager::SRC::PLAYER).handleId_);
}

void Player::InitTransform(void)
{
	//モデルの基本設定
	transform_.scl = AsoUtility::VECTOR_ONE;
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Euler(DEFAULT_ROT_LOCAL);
	transform_.pos = VGet(2000.0f,50.0f,2000.0f);
	transform_.Update();
}

void Player::InitCollider(void)
{
	// 主に地面との衝突で仕様する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::PLAYER, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Player::InitAnimation(void)
{
	//アニメーションコントローラー
	animationController_ =
		new AnimationController(transform_.modelId);

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::IDLE), 20.0f,
		Application::PATH_MODEL + "Player/Idle.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::RUN), 20.0f,
		Application::PATH_MODEL + "Player/Run.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::FAST_RUN), 20.0f,
		Application::PATH_MODEL + "Player/FastRun.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::JUMP), 60.0f,
		Application::PATH_MODEL + "Player/JumpRising.mv1");

	//初期アニメーション再生
	animationController_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void Player::InitPost(void)
{
}

void Player::UpdateProcess(void)
{
	// 移動操作
	ProcessMove();

	//ジャンプ処理
	ProcessJump();
}

void Player::UpdateProcessPost(void)
{
}

void Player::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();

	//移動量
	movePow_ = AsoUtility::Lerp(movePow_, AsoUtility::VECTOR_ZERO, 0.05f);

	//移動方向
	VECTOR dir = AsoUtility::VECTOR_ZERO;

	//ダッシュボタン
	bool isDash = false;

	//ゲームパッドが接続されている数で処理を分ける
	if (GetJoypadNum() == 0)
	{
		//キーボード操作
		if (ins.IsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }
		//右Shiftでダッシュ
		if (ins.IsNew(KEY_INPUT_RSHIFT)) { isDash = true; }

	}
	else
	{
		//ゲームパッド操作
		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// アナログキーの入力値から方向を取得
		dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

		//右Shiftでダッシュ
		if (ins.IsPadBtnNew(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::R_TRIGGER)) {
			isDash = true;
		}
	}

	if (!AsoUtility::EqualsVZero(dir))
	{

		//移動スピード
		moveSpeed_ = SPEED_MOVE;
		if (isDash)
		{
			//移動スピード
			moveSpeed_ = SPEED_DASH;
		}

		//ジャンプ中はアニメーションを変えない
		if (!isJump_)
		{
			//アニメーション
			if (isDash)
			{
				animationController_->Play(
					static_cast<int>(ANIM_TYPE::FAST_RUN), true);
			}
			else
			{
				animationController_->Play(
					static_cast<int>(ANIM_TYPE::RUN), true);
			}
		}

		//Y軸のみのカメラ角度を取得
		Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();

		//移動方向をカメラに合わせる
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		//移動量を計算
		movePow_ = VScale(moveDir_, moveSpeed_);

	}
	else
	{
		//ジャンプ中はアニメーションを変えない
		if (!isJump_)
		{
			//IDLE状態に戻す
			animationController_->Play(
				static_cast<int>(ANIM_TYPE::IDLE), true);
		}
	}

}

void Player::ProcessJump(void)
{
	auto& ins = InputManager::GetInstance();

	// 持続ジャンプ処理
	bool isHitKeyNew = ins.IsNew(KEY_INPUT_BACKSLASH)
		|| ins.IsPadBtnNew(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	if (isHitKeyNew)
	{
		// ジャンプの入力受付時間を減少
		stepJump_ += scnMng_.GetDeltaTime();

		if (stepJump_ < TIME_JUMP_INPUT)
		{
			// ジャンプ量の計算
			float jumpSpeed = POW_JUMP_KEEP * scnMng_.GetDeltaTime();
			jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
		}
	}
	else
	{
		// ボタンを離したらジャンプ力に加算しない
		stepJump_ = TIME_JUMP_INPUT;
	}

	// 初期ジャンプ処理
	bool isHitKey = ins.IsTrgDown(KEY_INPUT_BACKSLASH)
		|| ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	// ジャンプ
	if (isHitKey && !isJump_)
	{
		// ジャンプ量の計算
		float jumpSpeed = POW_JUMP_INIT * scnMng_.GetDeltaTime();
		jumpPow_ = VScale(AsoUtility::DIR_U, jumpSpeed);
		isJump_ = true;

		// アニメーション再生
		animationController_->Play(
			static_cast<int>(ANIM_TYPE::JUMP), false);
	}

}

void Player::CollisionReserve(void)
{
	//アニメーションごとの線分調整
	if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
	{
		//ジャンプ中は線分を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
		}

		//ジャンプ中はカプセルを上に上げる
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_JUMP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_JUMP_LOCAL_POS);
			colCapsule->SetRadius(COL_CAPSULE_RADIUS);
		}
	}
	else
	{

		//通常時の通常時に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
		}

		//通常時のカプセルに戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
			colCapsule->SetRadius(COL_CAPSULE_RADIUS);
		}
	}
}
