#include "Boss.h"

#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Application.h"

#include "../../../Common/AnimationController.h"

#include "../../../../Object/Collider/ColliderLine.h"
#include "../../../../Object/Collider/ColliderCapsule.h"

Boss::Boss()
	:
	EnemyBase()
{
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
}

void Boss::InitTransform()
{
	transform_.scl = VGet(2.0f,2.0f,2.0f);

	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();

	// 出現位置
	transform_.pos = VGet(2500.0f, 2900.0f, 2500.0f);

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
}

void Boss::InitAnimation()
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

void Boss::InitPost()
{
}

void Boss::AI()
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

void Boss::Damage(int damage)
{
    hp_ -= damage;


    if (hp_ <= 0)
    {
        hp_ = 0;

        isDead_ = true;

    }
}

bool Boss::IsDead() const
{
    return isDead_;
}
