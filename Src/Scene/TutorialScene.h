#pragma once
#include "SceneBase.h"

class Player;
class Stage;
class EnemyMob;
class TutorialUI;
class WeaponManager;

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

    Player* player_ = nullptr;
    WeaponManager* weaponManager_ = nullptr;
    Stage* stage_ = nullptr;
    EnemyMob* enemy_ = nullptr;
    TutorialUI* tutorialUI_ = nullptr;

    static constexpr int WALK_REQUIRED = 120;
    static constexpr int RUN_REQUIRED = 120;

    void CheckTutorialInput();
    int  walkFrames_ = 0;
    int  runFrames_ = 0;

    void  CheckAreaLimit();
    bool  IsOutOfArea() const;

    VECTOR areaCenter_ = { 0.0f, 0.0f, 0.0f };
    float  areaRadius_ = 3000.0f;

    int  warningCount_ = 0;
    static constexpr int WARNING_CHANGE_COUNT = 3;
    static constexpr const char* WARNING_MSG_NORMAL = "どけ行っど！戻ってけ！";
    static constexpr const char* WARNING_MSG_STRICT = "もうよか！わいは剣士じゃなか！";

    void  UpdateAfterWarning();
    void  OnAfterWarning();
    bool  waitingAfterWarning_ = false;
    int   afterWarningTimer_ = 0;
    static constexpr int AFTER_WARNING_DELAY = 15;
};