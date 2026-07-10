#include "../../../Utility/AsoUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/SoundManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Manager/WeaponManager.h"
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
    CharactorBase::InitLoad();

    transform_.SetModel(
        resMng_.Load(ResourceManager::SRC::PLAYER).handleId_);
}

void Player::InitTransform(void)
{
    transform_.scl = AsoUtility::VECTOR_ONE;
    transform_.quaRot = Quaternion::Identity();
    transform_.quaRotLocal = Quaternion::Euler(DEFAULT_ROT_LOCAL);
    transform_.pos = VGet(2000.0f, 50.0f, 2000.0f);
    transform_.Update();
}

void Player::InitCollider(void)
{
    ColliderLine* colLine = new ColliderLine(
        ColliderBase::TAG::PLAYER, &transform_,
        COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

    ColliderCapsule* colCapsule = new ColliderCapsule(
        ColliderBase::TAG::PLAYER, &transform_,
        COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
        COL_CAPSULE_RADIUS);
    ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void Player::InitAnimation(void)
{
    animationController_ = new AnimationController(transform_.modelId);

    animationController_->Add(static_cast<int>(ANIM_TYPE::IDLE), 20.0f, Application::PATH_MODEL + "Charactor/Player/Idle.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::RUN), 20.0f, Application::PATH_MODEL + "Charactor/Player/Run.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN), 20.0f, Application::PATH_MODEL + "Charactor/Player/FastRun.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::JUMP), 60.0f, Application::PATH_MODEL + "Charactor/Player/Jump.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK), 80.0f, Application::PATH_MODEL + "Charactor/Player/Attack.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::RELOAD), 80.0f, Application::PATH_MODEL + "Charactor/Player/Reload.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWITCH_OUT), 60.0f, Application::PATH_MODEL + "Charactor/Player/KatanaIn.mv1");
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWITCH_IN), 60.0f, Application::PATH_MODEL + "Charactor/Player/KatanaIn.mv1");

    animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
    animType_ = ANIM_TYPE::IDLE;
}

void Player::InitPost(void)
{
}

void Player::SetWeaponManager(WeaponManager* weaponMng)
{
    weaponMng_ = weaponMng;
}

void Player::UpdateProcess(void)
{
    if (isSwitching_)
    {
        ProcessSwitch();
        transform_.Update();
        return;
    }

    ProcessMove();
    ProcessJump();
    ProcessAttack();
    ProcessReload();
    ProcessWeaponSwitch();

    if (animType_ != ANIM_TYPE::RUN && animType_ != ANIM_TYPE::FAST_RUN)
    {
        sndMng_.Stop(SoundManager::SRC::Walk);
        sndMng_.Stop(SoundManager::SRC::Run);
    }

    transform_.Update();
}

void Player::UpdateProcessPost(void)
{
}

void Player::ProcessWeaponSwitch(void)
{
    if (isAttack_ || isReload_) return;

    auto& ins = InputManager::GetInstance();

    bool switchKey = ins.IsTrgDown(KEY_INPUT_E)
        || ins.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::TOP);

    if (!switchKey) return;

    BeginSwitchOut();
}

void Player::BeginSwitchOut(void)
{
    movePow_ = { 0.0f,0.0f,0.0f };
    isSwitching_ = true;
    isAttack_ = false;

    animationController_->Play(static_cast<int>(ANIM_TYPE::SWITCH_OUT), false);
    animType_ = ANIM_TYPE::SWITCH_OUT;
}

void Player::BeginSwitchIn(void)
{
    animationController_->Play(static_cast<int>(ANIM_TYPE::SWITCH_IN), false);
    animType_ = ANIM_TYPE::SWITCH_IN;
}

void Player::ProcessSwitch(void)
{
    if (!animationController_->IsEnd()) return;

    movePow_ = { 0.0f,0.0f,0.0f };

    if (animType_ == ANIM_TYPE::SWITCH_OUT)
    {
        auto result = weaponMng_->ApplyWeaponSwitch();

        if (result == WeaponManager::SwitchResult::BEGIN_SWITCH_IN)
        {
            OnWeaponChanged();
            BeginSwitchIn();
        }
    }
    else if (animType_ == ANIM_TYPE::SWITCH_IN)
    {
        isSwitching_ = false;
        animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
        animType_ = ANIM_TYPE::IDLE;
    }
}

void Player::OnWeaponChanged(void)
{
    if (weaponMng_ == nullptr) return;

    animationController_->Release();
    delete animationController_;
    animationController_ = new AnimationController(transform_.modelId);

    animationController_->Add(static_cast<int>(ANIM_TYPE::IDLE), 20.0f, Application::PATH_MODEL + weaponMng_->GetIdleAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::RUN), 20.0f, Application::PATH_MODEL + weaponMng_->GetRunAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN), 20.0f, Application::PATH_MODEL + weaponMng_->GetFastRunAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::JUMP), 60.0f, Application::PATH_MODEL + weaponMng_->GetJumpAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK), 100.0f, Application::PATH_MODEL + weaponMng_->GetAttackAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::RELOAD), 80.0f, Application::PATH_MODEL + weaponMng_->GetReloadAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWITCH_OUT), 60.0f, Application::PATH_MODEL + weaponMng_->GetSwitchOutAnimPath());
    animationController_->Add(static_cast<int>(ANIM_TYPE::SWITCH_IN), 60.0f, Application::PATH_MODEL + weaponMng_->GetSwitchInAnimPath());
}

WEAPON_STATE Player::GetWeaponState(void) const
{
    switch (animType_)
    {
    case ANIM_TYPE::RUN:
        return WEAPON_STATE::MOVE;
    case ANIM_TYPE::FAST_RUN:
        return WEAPON_STATE::DASH;
    case ANIM_TYPE::JUMP:
        return WEAPON_STATE::JUMP;
    case ANIM_TYPE::ATTACK:
        return WEAPON_STATE::ATTACK;
    case ANIM_TYPE::RELOAD:
        return WEAPON_STATE::RELOAD;
    default:
        return WEAPON_STATE::IDLE;
    }
}

bool Player::IsSwitching(void) const
{
    return isSwitching_;
}

ColliderBase* Player::GetCollider(int type)
{
    if (ownColliders_.count(type) == 0) return nullptr;
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

bool Player::IsDead(void) const
{
    return isDead_;
}

Player::ANIM_TYPE Player::GetAnimType(void) const
{
    return animType_;
}

void Player::ProcessMove(void)
{
    if (isAttack_ || isReload_ || isSwitching_) return;

    auto& ins = InputManager::GetInstance();

    movePow_ = AsoUtility::Lerp(movePow_, AsoUtility::VECTOR_ZERO, 0.05f);

    VECTOR dir = AsoUtility::VECTOR_ZERO;
    bool isDash = false;

    if (GetJoypadNum() == 0)
    {
        if (ins.IsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
        if (ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
        if (ins.IsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
        if (ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }
        if (CheckHitKey(KEY_INPUT_LSHIFT)) { isDash = true; }
    }
    else
    {
        InputManager::JOYPAD_IN_STATE padState =
            ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
        dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
        if (ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER))
        {
            isDash = true;
        }
    }

    if (!AsoUtility::EqualsVZero(dir))
    {
        moveSpeed_ = isDash ? SPEED_DASH : SPEED_MOVE;

        if (!isJump_)
        {
            if (isDash)
            {
                animationController_->Play(static_cast<int>(ANIM_TYPE::FAST_RUN), true);
                animType_ = ANIM_TYPE::FAST_RUN;
                sndMng_.Play(SoundManager::SRC::Run, false);
                sndMng_.Stop(SoundManager::SRC::Walk);
            }
            else
            {
                animationController_->Play(static_cast<int>(ANIM_TYPE::RUN), true);
                animType_ = ANIM_TYPE::RUN;
                sndMng_.Play(SoundManager::SRC::Walk, false);
                sndMng_.Stop(SoundManager::SRC::Run);
            }
        }

        Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();
        moveDir_ = Quaternion::PosAxis(cameraRot, dir);
        movePow_ = VScale(moveDir_, moveSpeed_);

        if (isSteepSlope_) { movePow_ = VScale(movePow_, 0.5f); }
    }
    else
    {
        if (!isJump_)
        {
            animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE), true);
            animType_ = ANIM_TYPE::IDLE;
        }
    }
}

void Player::ProcessJump(void)
{
    auto& ins = InputManager::GetInstance();

    bool isHitKeyNew = ins.IsNew(KEY_INPUT_SPACE)
        || ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

    if (isHitKeyNew)
    {
        stepJump_ += scnMng_.GetDeltaTime();
        if (stepJump_ < TIME_JUMP_INPUT)
        {
            float jumpSpeed = POW_JUMP_KEEP * scnMng_.GetDeltaTime();
            jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
        }
    }
    else
    {
        stepJump_ = TIME_JUMP_INPUT;
    }

    bool isHitKey = ins.IsTrgDown(KEY_INPUT_SPACE)
        || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

    if (isHitKey && !isJump_)
    {
        float jumpSpeed = POW_JUMP_INIT * scnMng_.GetDeltaTime();
        jumpPow_ = VScale(AsoUtility::DIR_U, jumpSpeed);
        isJump_ = true;

        animationController_->Play(static_cast<int>(ANIM_TYPE::JUMP), false);
        animType_ = ANIM_TYPE::JUMP;
    }
}

void Player::ProcessAttack(void)
{
    auto& ins = InputManager::GetInstance();

    if (GetMouseInput() & (MOUSE_INPUT_LEFT) && !isAttack_ ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::R_TRIGGER) && !isAttack_)
    {
        isAttack_ = true;
        attackTimer_ = weaponMng_->GetAttackDuration();

        animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
        animType_ = ANIM_TYPE::ATTACK;
        if (weaponMng_->GetActiveWeaponType() == WeaponManager::WEAPON_TYPE::GUN)
        {
            sndMng_.Play(SoundManager::SRC::Attack);
        }
        else {
            sndMng_.Play(SoundManager::SRC::Attack);
        }

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

void Player::CollisionReserve(void)
{
    if (animationController_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
    {
        if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
        {
            ColliderLine* colLine = dynamic_cast<ColliderLine*>(
                ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
            colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
            colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
        }
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
        if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
        {
            ColliderLine* colLine = dynamic_cast<ColliderLine*>(
                ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
            colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
            colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
        }
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

void Player::ProcessReload(void)
{
    if (weaponMng_->GetActiveWeaponType() != WeaponManager::WEAPON_TYPE::GUN) return;

    auto& ins = InputManager::GetInstance();


    if (ins.IsNew(KEY_INPUT_Q) && !isAttack_ && !isReload_ ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::LEFT) && !isAttack_ && !isReload_)
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

        // IDLE‚É–ß‚·
        animationController_->Play(
            static_cast<int>(ANIM_TYPE::IDLE), true);
        animType_ = ANIM_TYPE::IDLE;
    }
}

