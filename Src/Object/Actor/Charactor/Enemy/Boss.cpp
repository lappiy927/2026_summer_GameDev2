#include "Boss.h"

#include <EffekseerForDXLib.h>
#include "../Player.h"

#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/SoundManager.h"
#include "../../../../Application.h"

#include "../../../Common/AnimationController.h"

#include "../../../../Object/Collider/ColliderLine.h"
#include "../../../../Object/Collider/ColliderCapsule.h"

Boss::Boss()
    :
    EnemyBase()
{
    hp_ = 500;

    attackRange_ = 400.0f;
    searchRange_ = 3000.0f;
}

Boss::~Boss()
{

}

void Boss::InitLoad()
{
    // 共通ロード
    CharactorBase::InitLoad();

    // モデル読み込み
    int model =
        MV1DuplicateModel(
            resMng_.Load(
                ResourceManager::SRC::BOSS).handleId_);

    transform_.SetModel(model);

    effectHandle = LoadEffekseerEffect(_T("Data/effect/blood.efk"), 50.0f);

}

void Boss::InitTransform()
{
    transform_.scl = VGet(2.0f, 2.0f, 2.0f);

    transform_.quaRot = Quaternion::Identity();

    transform_.quaRotLocal = Quaternion::Identity();

    // 出現位置
    transform_.pos = VGet(2500.0f, 2000.0f, 2500.0f);

    transform_.Update();
}

void Boss::InitCollider()
{
    // 地面判定線
    ColliderLine* colLine =
        new ColliderLine(
            ColliderBase::TAG::BOSS,
            &transform_,
            VGet(0.0f, 100.0f, 0.0f),
            VGet(0.0f, -2000.0f, 0.0f));

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::LINE),
        colLine);

    // 壁判定カプセル
    ColliderCapsule* colCapsule =
        new ColliderCapsule(
            ColliderBase::TAG::BOSS,
            &transform_,
            VGet(0.0f, 120.0f, 0.0f),
            VGet(0.0f, 0.0f, 0.0f),
            80.0f);

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::CAPSULE),
        colCapsule);

    attackCollider_ =
        new ColliderCapsule(
            ColliderBase::TAG::BOSS,
            &transform_,
            VGet(0, 120, 80),
            VGet(0, 120, 250),
            60.0f);

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::ATTACK),
        attackCollider_);
}

void Boss::InitAnimation()
{
    animationController_ =
        new AnimationController(transform_.modelId);

    // 待機
    animationController_->Add(
        0,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/Boss/BossIdle.mv1");

    // 歩き
    animationController_->Add(
        1,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/Boss/BossWalk.mv1");

    // ダッシュ
    animationController_->Add(
        2,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/Boss/BossRun.mv1");


    // 攻撃
    animationController_->Add(
        3,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/Boss/BossAttack.mv1");

    // 死亡
    animationController_->Add(
        4,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/Boss/BossDai.mv1");

    // 初期アニメ
    animationController_->Play(0, true);
}

void Boss::InitPost()
{
}

void Boss::AI()
{
    if (isTackle_)
    {
        return;
    }


    // 行動中ならAIで状態を変えない
    if (state_ == STATE::DASH_READY ||
        state_ == STATE::DASH)
    {
        return;
    }



    if (target_ == nullptr)
    {
        return;
    }

    // 攻撃中なら他の行動をしない
    if (isAttacking_)
    {
        if (animationController_->IsEnd())
        {
            isAttacking_ = false;
            state_ = STATE::CHASE;

            animationController_->Play(1, true);
        }

        return;
    }

    float dist = GetPlayerDistance();

    if (state_ == STATE::DEAD)
    {
        animationController_->Play(4, false);
        return;
    }

    if (dist <= attackRange_)
    {
        state_ = STATE::ATTACK;

        isAttacking_ = true;
        animationController_->Play(3, false);
    }
    else if (dist <= 800.0f)
    {
        if (state_ != STATE::CHASE)
        {
            state_ = STATE::CHASE;
            animationController_->Play(1, true);
        }
    }
    else if (dist <= searchRange_)
    {
        if (state_ != STATE::DASH_READY)
        {
            state_ = STATE::DASH_READY;

            animationController_->Play(2, true);
        }
    }
    else
    {
        if (state_ != STATE::IDLE)
        {
            state_ = STATE::IDLE;
            animationController_->Play(0, true);
        }
    }

    attackEnable_ = (state_ == STATE::ATTACK);
}

void Boss::Damage(int damage)
{
    if (state_ == STATE::DEAD) return;

    hp_ -= damage;

    if (hp_ <= 0)
    {
        hp_ = 0;

        int shoulderFrame = MV1SearchFrame(transform_.modelId, "shoulder.L");
        if (shoulderFrame != -1)
        {
            MATRIX shoulderMatrix = MV1GetFrameLocalWorldMatrix(transform_.modelId, shoulderFrame);

            VECTOR rayOrigin = VGet(
                shoulderMatrix.m[3][0],
                shoulderMatrix.m[3][1],
                shoulderMatrix.m[3][2]);

            float yaw = transform_.quaRot.ToEuler().y;
            VECTOR forward = VGet(sinf(yaw), 0.0f, cosf(yaw));

            effectPos_ = VAdd(rayOrigin, forward);

            int playHandle = PlayEffekseer3DEffect(effectHandle);
            SetPosPlayingEffekseer3DEffect(playHandle, effectPos_.x, effectPos_.y, effectPos_.z);
            SetRotationPlayingEffekseer3DEffect(playHandle, 0.0f, yaw, 0.0f);
        }

        sndMng_.Play(SoundManager::SRC::EnemyDai);

        state_ = STATE::DEAD;
    }
}

bool Boss::IsAttack() const
{
    return attackEnable_;
}