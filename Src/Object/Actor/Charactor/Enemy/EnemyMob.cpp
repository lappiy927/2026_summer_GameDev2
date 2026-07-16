#include "EnemyMob.h"
#include <EffekseerForDXLib.h>
#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include"../../../../Manager/SoundManager.h"
#include "../../../../Application.h"

#include "../../../Common/AnimationController.h"

#include "../../../../Object/Collider/ColliderLine.h"
#include "../../../../Object/Collider/ColliderCapsule.h"

EnemyMob::EnemyMob()
    :
    EnemyBase()
{
}

EnemyMob::~EnemyMob()
{

}

void EnemyMob::InitLoad()
{
    // 共通ロード
    CharactorBase::InitLoad();

    // モデル読み込み
    int model =
        MV1DuplicateModel(
            resMng_.Load(
                ResourceManager::SRC::ENEMY).handleId_);

    transform_.SetModel(model);

    effectHandle = LoadEffekseerEffect(_T("Data/effect/blood.efk"), 50.0f);
}

void EnemyMob::InitTransform()
{
    transform_.scl = AsoUtility::VECTOR_ONE;

    transform_.quaRot = Quaternion::Identity();

    transform_.quaRotLocal = Quaternion::Identity();

    // 出現位置
    transform_.pos = VGet(6000.0f, -1000.0f, 2300.0f);

    transform_.Update();
}

void EnemyMob::InitCollider()
{
    // 地面判定線
    ColliderLine* colLine =
        new ColliderLine(
            ColliderBase::TAG::ENEMY,
            &transform_,
            VGet(0.0f, 100.0f, 0.0f),
            VGet(0.0f, -2000.0f, 0.0f));

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::LINE),
        colLine);

    // 壁判定カプセル
    ColliderCapsule* colCapsule =
        new ColliderCapsule(
            ColliderBase::TAG::ENEMY,
            &transform_,
            VGet(0.0f, 120.0f, 0.0f),
            VGet(0.0f, 0.0f, 0.0f),
            40.0f);

    ownColliders_.emplace(
        static_cast<int>(COLLIDER_TYPE::CAPSULE),
        colCapsule);
}

void EnemyMob::InitAnimation()
{
    animationController_ =
        new AnimationController(transform_.modelId);

    // 待機
    animationController_->Add(
        0,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/EnemyAnime/EnemyIdle.mv1");

    // 走り
    animationController_->Add(
        1,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/EnemyAnime/EnemyRun.mv1");

    // 攻撃
    animationController_->Add(
        2,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/EnemyAnime/EnemyAttack.mv1");

    // 死
    animationController_->Add(
        3,
        20.0f,
        Application::PATH_MODEL + "Charactor/Enemy/EnemyAnime/EnemyDying.mv1");

    // 初期アニメ
    animationController_->Play(0, true);
}

void EnemyMob::InitPost()
{
}

void EnemyMob::AI()
{
    switch (state_)
    {
    case STATE::IDLE:

        animationController_->Play(0, true);

        break;

    case STATE::CHASE:

        animationController_->Play(1, true);

        break;

    case STATE::ATTACK:
        animationController_->Play(2, true);
        break;

    case STATE::DEAD:
        animationController_->Play(3, false);
        break;
    }
}

void EnemyMob::Damage(int damage)
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
