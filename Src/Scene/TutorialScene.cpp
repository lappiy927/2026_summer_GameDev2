#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/WeaponManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/Stage.h"
#include"../Object/Actor/Stage/Grass.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Actor/Charactor/Enemy/EnemyMob.h"
#include "../Object/TutorialUI.h"
#include "TutorialScene.h"

TutorialScene::TutorialScene(void) : SceneBase()
{
}

TutorialScene::~TutorialScene(void)
{
}

void TutorialScene::Init(void)
{
    player_ = new Player();
    player_->Init();

    weaponManager_ = new WeaponManager();
    weaponManager_->Init();

    player_->SetWeaponManager(weaponManager_);

    stage_ = new Stage();
    stage_->Init();

    grass_ = std::make_unique<Grass>();
    grass_->Init();

    grass_->AddHitCollider(stage_->GetModelCollider());
    grass_->GenerateField(10000, 10000.0f);

    enemy_ = new EnemyMob();
    enemy_->Init();
    enemy_->SetPos({ 2300.0f, 0.0f, 2300.0f });

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

    // ---- スキップ長押し（Escape / スタートボタン を1秒）----
    bool holdingEsc = ins.IsNew(KEY_INPUT_ESCAPE) != 0;
    bool holdingStart = (GetJoypadNum() > 0) &&
        ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::MENU);
    if (!tutorialUI_->IsFinished() &&
        tutorialUI_->UpdateSkipHold(holdingEsc || holdingStart))
    {
        sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        return;
    }

    tutorialUI_->Update();

    // ---- COMPLETE（遷移待ち）----
    if (tutorialUI_->IsFinished())
    {
        bool endTutorial = ins.IsTrgDown(KEY_INPUT_SPACE) ||
            ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
                InputManager::JOYPAD_BTN::RIGHT);
        if (endTutorial) sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        return;
    }

    // ---- プレイヤー・ステージ・武器は ENDING 中も常に更新 ----
    player_->Update();
    stage_->Update();

    grass_->SetPlayerPos(player_->GetPos());
    grass_->Update();

    weaponManager_->Update(player_->GetTransform(), player_->GetWeaponState());

    // ---- ENDING（完了セリフ中・自由移動）----
    if (tutorialUI_->IsEnding())
    {
        bool endTutorial = ins.IsTrgDown(KEY_INPUT_SPACE) ||
            ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
                InputManager::JOYPAD_BTN::RIGHT);
        if (endTutorial) sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        return;
    }

    // ---- チュートリアルステップ中の更新 ----
    CheckTutorialInput();
    CheckAreaLimit();
    UpdateAfterWarning();

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
            if (weaponManager_->GetActiveCollider()->IsHit(enemyCol))
            {
                enemy_->Damage(50);
            }
        }
    }
}

void TutorialScene::Draw(void)
{
    stage_->Draw();
    grass_->Draw();
    player_->Draw();
    weaponManager_->Draw();
   

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

    weaponManager_->Release();
    delete weaponManager_;

    stage_->Release();
    delete stage_;

    enemy_->Release();
    delete enemy_;

    tutorialUI_->Release();
    delete tutorialUI_;

    grass_->Release();
}

bool TutorialScene::IsOutOfArea() const
{
    VECTOR pos = player_->GetPos();
    float dx = pos.x - areaCenter_.x;
    float dz = pos.z - areaCenter_.z;
    return (dx * dx + dz * dz) > (areaRadius_ * areaRadius_);
}

void TutorialScene::CheckAreaLimit()
{
    if (!IsOutOfArea()) return;

    player_->SetPos(areaCenter_);

    warningCount_++;

    if (warningCount_ < WARNING_CHANGE_COUNT)
    {
        tutorialUI_->ShowWarning(WARNING_MSG_NORMAL);
    }
    else
    {
        // 強化警告：セリフ終了後にディレイ処理を起動
        tutorialUI_->ShowWarning(WARNING_MSG_STRICT,
            [this]()
            {
                waitingAfterWarning_ = true;
                afterWarningTimer_ = AFTER_WARNING_DELAY;
            });
    }
}

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

    case TutorialStep::WEAPON_CHANGE:
    {
        if (weaponManager_->GetActiveWeaponType() != WeaponManager::WEAPON_TYPE::KATANA)
        {
            tutorialUI_->NotifyWeaponChangeSuccess();
        }
        break;
    }

    default:
        break;
    }
}

void TutorialScene::UpdateAfterWarning()
{
    if (!waitingAfterWarning_) return;

    afterWarningTimer_--;
    if (afterWarningTimer_ <= 0)
    {
        waitingAfterWarning_ = false;
        OnAfterWarning();
    }
}

void TutorialScene::OnAfterWarning()
{
    SetEndRequest();
}