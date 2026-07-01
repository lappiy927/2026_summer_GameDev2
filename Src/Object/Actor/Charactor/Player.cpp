#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include"../../../Manager/SoundManager.h"
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
	isPlayer_ = true;
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
	transform_.pos = VGet(2000.0f, 50.0f, 2000.0f);
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
		Application::PATH_MODEL + "Charactor/Player/Idle.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::RUN), 20.0f,
		Application::PATH_MODEL + "Charactor/Player/Run.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::FAST_RUN), 20.0f,
		Application::PATH_MODEL + "Charactor/Player/FastRun.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::JUMP), 60.0f,
		Application::PATH_MODEL + "Charactor/Player/Jump.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::ATTACK), 80.0f,
		Application::PATH_MODEL + "Charactor/Player/Attack.mv1");

	animationController_->Add(
		static_cast<int>(ANIM_TYPE::RELOAD), 60.0f,
		Application::PATH_MODEL + "Charactor/Player/Reload.mv1");

	//初期アニメーション再生
	animationController_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
	animType_ = ANIM_TYPE::IDLE;
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

	ProcessAttack();

	ProcessReload();

	if (animType_ != ANIM_TYPE::RUN && animType_ != ANIM_TYPE::FAST_RUN) {
		sndMng_.Stop(SoundManager::SRC::Walk);
		sndMng_.Stop(SoundManager::SRC::Run);
	}

	transform_.Update();
}

void Player::UpdateProcessPost(void)
{
}

ColliderBase* Player::GetCollider(int type)
{
	if (ownColliders_.count(type) == 0)
		return nullptr;

	return ownColliders_.at(type);
}

void Player::SetPos(const VECTOR& pos)
{
	transform_.pos = pos;
}

void Player::Damage(int power)
{
	hp_ -= power;

	if (hp_ <= 0)
	{
		hp_ = 0;
		isDead_ = true;
	}
}

bool Player::IsDead() const
{
	return isDead_;
}

Player::ANIM_TYPE Player::GetAnimType() const
{
	return animType_;
}

void Player::ProcessMove(void)
{
	if (isAttack_)
	{
		return;
	}
	if (isReload_) return;

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

		//左Shiftでダッシュ
		if (CheckHitKey(KEY_INPUT_LSHIFT))
		{
			isDash = true;
		}
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
			InputManager::JOYPAD_BTN::L_TRIGGER)) {
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
				animType_ = ANIM_TYPE::FAST_RUN;
				sndMng_.Play(SoundManager::SRC::Run, false);
				sndMng_.Stop(SoundManager::SRC::Walk);
			}
			else
			{
				animationController_->Play(
					static_cast<int>(ANIM_TYPE::RUN), true);
				animType_ = ANIM_TYPE::RUN;
				sndMng_.Play(SoundManager::SRC::Walk, false);
				sndMng_.Stop(SoundManager::SRC::Run);
			}
		}

		//Y軸のみのカメラ角度を取得
		Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();

		//移動方向をカメラに合わせる
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		//移動量を計算
		movePow_ = VScale(moveDir_, moveSpeed_);

		if (isSteepSlope_)
		{
			movePow_ = VScale(movePow_, 0.5f);
		}
	}
	else
	{
		//ジャンプ中はアニメーションを変えない
		if (!isJump_)
		{
			//IDLE状態に戻す
			animationController_->Play(
				static_cast<int>(ANIM_TYPE::IDLE), true);
			animType_ = ANIM_TYPE::IDLE;
		}
	}
}

void Player::ProcessJump(void)
{
	auto& ins = InputManager::GetInstance();

	// 持続ジャンプ処理
	bool isHitKeyNew = ins.IsNew(KEY_INPUT_SPACE)
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
	bool isHitKey = ins.IsTrgDown(KEY_INPUT_SPACE)
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
		animType_ = ANIM_TYPE::JUMP;
	}

}

void Player::ProcessAttack(void)
{
	auto& ins = InputManager::GetInstance();

	bool mouse =
		(GetMouseInput() & MOUSE_INPUT_LEFT);

	bool padAttack =
		ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::R_TRIGGER);

	if ((mouse || padAttack) && !isAttack_)
	{
		isAttack_ = true;

		attackTimer_ = 1.0f;

		animationController_->Play(
			static_cast<int>(ANIM_TYPE::ATTACK),
			false);
		animType_ = ANIM_TYPE::ATTACK;
		sndMng_.Play(SoundManager::SRC::Attack);
	}

	if (isAttack_)
	{
		movePow_ = AsoUtility::VECTOR_ZERO;
		attackTimer_ -= scnMng_.GetDeltaTime();

		if (attackTimer_ <= 0.0f)
		{
			isAttack_ = false;
		}
	}
}

void Player::ProcessReload(void)
{
	auto& ins = InputManager::GetInstance();


	if (ins.IsNew(KEY_INPUT_Q) && !isAttack_ && !isReload_)
	{
		animationController_->Play(
			static_cast<int>(ANIM_TYPE::RELOAD), false);
		animType_ = ANIM_TYPE::RELOAD;
		isReload_ = true;
		reloadTimer_ = RELOAD_ANIM_TIME;
	}

	if (!isReload_) return;
	movePow_ = { 0.0f,0.0f,0.0f };

	reloadTimer_ -= scnMng_.GetDeltaTime();

	if (reloadTimer_ <= 0.0f)
	{
		isReload_ = false;
		reloadTimer_ = 0.0f;

		// IDLEに戻す
		animationController_->Play(
			static_cast<int>(ANIM_TYPE::IDLE), true);
		animType_ = ANIM_TYPE::IDLE;
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
