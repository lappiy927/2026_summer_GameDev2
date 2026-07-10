#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include"../Manager/WeaponManager.h"
#include"../Manager/SoundManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Charactor/Player.h"
#include"../Object/Actor/Charactor/Enemy/EnemyMob.h"
#include "../Object/Actor/Weapon/Gun.h"
#include"../Object/TutorialUI.h"
#include "TutorialScene.h"



TutorialScene::TutorialScene(void) : SceneBase(),
player_(nullptr),
weaponMng_(nullptr),
stage_(nullptr),
enemy_(nullptr),
tutorialUI_(nullptr),
walkFrames_(0),
runFrames_(0),
prevMouseLeft_(false)
{
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
{
    // 残弾数をリセット
    Gun::ResetRemainingBullets();

    player_ = new Player();
    player_->Init();

    weaponMng_ = new WeaponManager();
    weaponMng_->Init();
    player_->SetWeaponManager(weaponMng_);

    stage_ = new Stage();
    stage_->Init();

    enemy_ = new EnemyMob();
    enemy_->Init();

    const ColliderBase* stageCollider =
        stage_->GetOwnCollider(static_cast<int>(Stage::COLLIDER_TYPE::MODEL));
    player_->AddHitCollider(stageCollider);
    enemy_->AddHitCollider(stageCollider);

    Camera* camera = sceMng_.GetCamera();
    camera->SetFollow(&player_->GetTransform());
    camera->ChangeMode(Camera::MODE::FOLLOW);

    tutorialUI_ = new TutorialUI();
    tutorialUI_->Init();

    areaCenter_ = player_->GetPos();
}

void TutorialScene::Update(void)
{
    auto& ins = InputManager::GetInstance();

    bool holdingEsc = ins.IsNew(KEY_INPUT_ESCAPE) != 0;
    bool holdingStart = (GetJoypadNum() > 0) &&
        ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::MENU);
    if (!tutorialUI_->IsFinished() && tutorialUI_->UpdateSkipHold(holdingEsc || holdingStart))
    {
        sndMng_.StopAll();
        sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        return;
    }

    tutorialUI_->Update();

    // ---- チュートリアル完了後 ----
    if (tutorialUI_->IsFinished())
    {
        InputManager::JOYPAD_IN_STATE padState =
            ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

        bool endTutorial = ins.IsTrgDown(KEY_INPUT_SPACE) ||
            ins.IsTrgDown(KEY_INPUT_RETURN)||
            ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
                InputManager::JOYPAD_BTN::RIGHT);
        if (endTutorial)
        {
            sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        }
        return;
    }

    CheckTutorialInput();
    CheckAreaLimit();

    if (!isAnger_) {


        // ---- 通常更新 ----
        player_->Update();
        weaponMng_->Update(
            player_->GetTransform(),
            player_->GetWeaponState());
        stage_->Update();

        // 攻撃ステップ以降は敵を更新
        if (TutorialStep::ATTACK <= tutorialUI_->GetCurrentStep())
        {
            enemy_->Update();

            ColliderCapsule* enemyCol =
                dynamic_cast<ColliderCapsule*>(
                    enemy_->GetCollider(
                        static_cast<int>(EnemyBase::COLLIDER_TYPE::CAPSULE)));

            if (enemyCol != nullptr)
            {
                if (weaponMng_->GetActiveCollider()->IsHit(enemyCol))
                {
                    enemy_->Damage(50);
                }
            }
        }
    }
    else {

        if (++endCount_ == 80) {
            SetEndRequest();
        }
    }
}

void TutorialScene::Draw(void)
{
    player_->Draw();
    weaponMng_->Draw();
    stage_->Draw();

    if (TutorialStep::ATTACK <= tutorialUI_->GetCurrentStep() && !enemy_->IsDead())
    {
        enemy_->Draw();
    }

    tutorialUI_->Draw();
}

void TutorialScene::Release(void)
{
    player_->Release();
    delete player_;

    weaponMng_->Release();
    delete weaponMng_;

    stage_->Release();
    delete stage_;

    tutorialUI_->Release();
    delete tutorialUI_;

    enemy_->Release();
    delete enemy_;
}

// ============================================================
//  行動範囲チェック
// ============================================================
bool TutorialScene::IsOutOfArea() const
{
    VECTOR pos = player_->GetPos();
    float dx = pos.x - areaCenter_.x;
    float dz = pos.z - areaCenter_.z;
    return (dx * dx + dz * dz) > (areaRadius_ * areaRadius_);
}

void TutorialScene::CheckAreaLimit()
{
    if (AngerCount_ < 3) {
        if (!IsOutOfArea()) return;

        player_->SetPos(areaCenter_);
        tutorialUI_->ShowWarning("どけ行っど！戻ってけ！");
        AngerCount_++;
    }
    else {
        if (!IsOutOfArea()) return;

        tutorialUI_->ShowWarning("もうよか！わいは剣士じゃなか！");
        isAnger_ = true;
    }
}

// ============================================================
//  チュートリアル入力チェック
// ============================================================
void TutorialScene::CheckTutorialInput()
{
    auto& ins = InputManager::GetInstance();

    InputManager::JOYPAD_IN_STATE padState =
        ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

    switch (tutorialUI_->GetCurrentStep())
    {
    case TutorialStep::WALK:
    {
        VECTOR moveDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
        bool LTrigger = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::L_TRIGGER);
        bool wasd = ins.IsNew(KEY_INPUT_W) || ins.IsNew(KEY_INPUT_A)
            || ins.IsNew(KEY_INPUT_S) || ins.IsNew(KEY_INPUT_D);
        bool shift = ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsNew(KEY_INPUT_RSHIFT);

        if ((wasd && !shift) ||
            (moveDir.x != 0.0f && !LTrigger) ||
            (moveDir.z != 0.0f && !LTrigger))
        {
            walkFrames_++;
        }

        if (walkFrames_ >= WALK_REQUIRED)
        {
            walkFrames_ = 0;
            tutorialUI_->NotifyWalkSuccess();
        }
        break;
    }

    case TutorialStep::RUN:
    {
        VECTOR moveDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
        bool LTrigger = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::L_TRIGGER);
        bool wasd = ins.IsNew(KEY_INPUT_W) || ins.IsNew(KEY_INPUT_A)
            || ins.IsNew(KEY_INPUT_S) || ins.IsNew(KEY_INPUT_D);
        bool shift = ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsNew(KEY_INPUT_RSHIFT);

        if ((wasd && shift) ||
            (moveDir.x != 0.0f && LTrigger) ||
            (moveDir.z != 0.0f && LTrigger))
        {
            runFrames_++;
        }

        if (runFrames_ >= RUN_REQUIRED)
        {
            runFrames_ = 0;
            tutorialUI_->NotifyRunSuccess();
        }
        break;
    }

    case TutorialStep::ATTACK:
    {
        if (enemy_->IsDead())
        {
            tutorialUI_->NotifyAttackSuccess();
        }
        break;
    }

    default:
        break;
    }
}
