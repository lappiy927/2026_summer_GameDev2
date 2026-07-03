#pragma once
#include "SceneBase.h"

class AnimationController;
class Player;
class WeaponManager;
class Stage;
class EnemyMob;
class TutorialUI;

class TutorialScene : public SceneBase
{
public:
    TutorialScene(void);
    ~TutorialScene(void) override;

    void Init(void)    override;
    void Update(void)  override;
    void Draw(void)    override;
    void Release(void) override;

private:
    Player* player_;
    WeaponManager* weaponMng_;
    Stage* stage_;
    EnemyMob* enemy_;
    TutorialUI* tutorialUI_;

    static constexpr int WALK_REQUIRED = 120;
    static constexpr int RUN_REQUIRED = 120;

    // --- 入力検知用 ---
    void CheckTutorialInput();
    int  walkFrames_ = 0;
    int  runFrames_ = 0;
    bool prevMouseLeft_ = false;

    //強制終了
    bool isAnger_ = false;
    int AngerCount_;
    int endCount_;

    // --- 行動範囲制限 ---
    void  CheckAreaLimit();
    bool  IsOutOfArea() const;

    VECTOR areaCenter_ = { 0.0f, 0.0f, 0.0f };
    float  areaRadius_ = 3000.0f;
};