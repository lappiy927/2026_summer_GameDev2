#include "EnemyMob.h"

#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
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
}

void EnemyMob::InitTransform()
{
	transform_.scl = AsoUtility::VECTOR_ONE;

	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	// 出現位置
	transform_.pos = VGet(2300.0f, 300.0f, 2300.0f);

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
        break;

    case STATE::DAMAGE:
        break;

    case STATE::DEAD:
        break;
    }
}

void EnemyMob::Damage(int damage)
{
    hp_ -= damage;


    if (hp_ <= 0)
    {
        hp_ = 0;

        isDead_ = true;

    }
}

bool EnemyMob::IsDead() const
{
    return isDead_;
}
