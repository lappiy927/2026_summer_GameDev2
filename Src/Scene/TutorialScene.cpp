#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Object/Actor/Stage/Stage.h"
#include "../Object/Actor/Charactor/Player.h"
#include"../Object/Actor/Charactor/Enemy/EnemyMob.h"
#include "../Object/Actor/Weapon/Katana.h"
#include"../Object/TutorialUI.h"
#include "TutorialScene.h"



TutorialScene::TutorialScene(void) : SceneBase(),
player_(nullptr),
katana_(nullptr),
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
    player_ = new Player();
    player_->Init();

    katana_ = new Katana(player_);
    katana_->Init();

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

    // チュートリアル UI 初期化
    tutorialUI_ = new TutorialUI();
    tutorialUI_->Init();

    areaCenter_ = player_->GetPos();
}

void TutorialScene::Update(void)
{
    // 完了後：Enterでゲームへ
    auto& ins = InputManager::GetInstance();

    // 接続されているゲームパッド１の情報を取得
    InputManager::JOYPAD_IN_STATE padState =
        ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

    if (tutorialUI_->IsFinished())
    {
        bool endTutorial = (ins.IsTrgDown(KEY_INPUT_RETURN) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::RIGHT));

        if (endTutorial) {
            sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
        }
    }
    else {

        player_->Update();
        katana_->Update();
        stage_->Update();

        tutorialUI_->Update();
        CheckTutorialInput();
        CheckAreaLimit();


        if (TutorialStep::ATTACK <= tutorialUI_->GetCurrentStep()) {

            enemy_->Update();

            ColliderCapsule* enemyCol =
                dynamic_cast<ColliderCapsule*>(
                    enemy_->GetCollider(
                        static_cast<int>(
                            EnemyBase::COLLIDER_TYPE::CAPSULE)));

            if (enemyCol != nullptr)
            {
                bool hit =
                    katana_->GetCollider()->IsHit(enemyCol);

                if (hit)
                {
                    enemy_->Damage(50);

                }
            };
        }
    }
}

void TutorialScene::Draw(void)
{
    player_->Draw();
    katana_->Draw();
    stage_->Draw();

    if (TutorialStep::ATTACK <= tutorialUI_->GetCurrentStep() && enemy_->IsDead() == false) {
        enemy_->Draw();
    }

    // UI は3D描画の後（最前面）
    tutorialUI_->Draw();
}

void TutorialScene::Release(void)
{
    player_->Release();
    delete player_;

    katana_->Release();
    delete katana_;

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

    // XZ平面での距離（高さは無視）
    float dx = pos.x - areaCenter_.x;
    float dz = pos.z - areaCenter_.z;
    float distSq = dx * dx + dz * dz;

    return distSq > (areaRadius_ * areaRadius_);
}

void TutorialScene::CheckAreaLimit()
{
    if (!IsOutOfArea()) return;

    // --- 瞬間テレポート（中心へ戻す）---
    player_->SetPos(areaCenter_);

    // --- 警告セリフを表示（TutorialUI 側で重複表示を防いでいる）---
    tutorialUI_->ShowWarning("どけ行っど！戻ってけ！");
}

// ============================================================
//  チュートリアル入力チェック
// ============================================================
void TutorialScene::CheckTutorialInput()
{
    auto& ins = InputManager::GetInstance();

    // 接続されているゲームパッド１の情報を取得
    InputManager::JOYPAD_IN_STATE padState =
        ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

    switch (tutorialUI_->GetCurrentStep())
    {
    case TutorialStep::WALK:
    {

        VECTOR moveDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);
        bool LTrigger = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

        bool wasd = ins.IsNew(KEY_INPUT_W) || ins.IsNew(KEY_INPUT_A)
            || ins.IsNew(KEY_INPUT_S) || ins.IsNew(KEY_INPUT_D);
        bool shift = ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsNew(KEY_INPUT_RSHIFT);

        if (wasd && !shift || moveDir.x != 0.0f && !LTrigger || moveDir.z != 0.0f && !LTrigger)
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
        bool LTrigger = ins.IsPadBtnNew(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::L_TRIGGER);

        bool wasd = ins.IsNew(KEY_INPUT_W) || ins.IsNew(KEY_INPUT_A)
            || ins.IsNew(KEY_INPUT_S) || ins.IsNew(KEY_INPUT_D);
        bool shift = ins.IsNew(KEY_INPUT_LSHIFT) || ins.IsNew(KEY_INPUT_RSHIFT);

        if (wasd && shift || moveDir.x != 0.0f && LTrigger || moveDir.z != 0.0f && LTrigger) {
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
        if (enemy_->IsDead() == true)
        {
            tutorialUI_->NotifyAttackSuccess();
        }

        break;
    }

    default:
        break;
    }
}
