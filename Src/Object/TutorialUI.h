#pragma once
#include <DxLib.h>
#include <string>
#include <vector>
#include <functional>

enum class TutorialStep
{
    WALK,
    RUN,
    ATTACK,
    WEAPON_CHANGE,
    GUN_INFO_1,
    GUN_INFO_2,
    ENDING,
    COMPLETE,
};

struct TutorialStepData
{
    TutorialStep step;
    std::string speakerName;
    std::string message;
    std::string hintText;
};

class TutorialUI
{
public:
    TutorialUI() = default;
    ~TutorialUI();

    void Init(void);
    void Update();
    void Draw() const;
    void Release();

    void NotifyWalkSuccess();
    void NotifyRunSuccess();
    void NotifyAttackSuccess();
    void NotifyWeaponChangeSuccess();
    void NotifyGunInfoNext();

    TutorialStep GetCurrentStep() const { return currentStep_; }
    bool IsFinished()  const { return currentStep_ == TutorialStep::COMPLETE; }
    bool IsEnding()    const { return currentStep_ == TutorialStep::ENDING; }
    bool IsGunInfo()   const {
        return currentStep_ == TutorialStep::GUN_INFO_1 ||
            currentStep_ == TutorialStep::GUN_INFO_2;
    }
    bool IsWarning()   const { return isWarning_; }

    void ShowWarning(const std::string& message,
        std::function<void()> onFinished = nullptr);
    bool UpdateSkipHold(bool holding);

    int screenW = 1280;
    int screenH = 720;

    std::function<void()> warningOnFinished_ = nullptr;

private:
    void AdvanceStep();
    void StartTypewriter(const std::string& text);
    void UpdateTypewriter();

    void DrawCharacter()       const;
    void DrawChatWindow()      const;
    void DrawProgressDots()    const;
    void DrawCompleteOverlay() const;
    void DrawSkipGauge()       const;

    int charImage_ = -1;
    int font_ = -1;
    int fontSmall_ = -1;

    std::vector<TutorialStepData> steps_;
    int stepIndex_ = 0;
    TutorialStep currentStep_ = TutorialStep::WALK;

    std::string fullText_;
    std::string shownText_;
    int  typeTimer_ = 0;
    int  typeInterval_ = 3;
    bool typingDone_ = false;

    int chatX_ = 0, chatY_ = 0, chatW_ = 0, chatH_ = 0;
    int charX_ = 0, charY_ = 0;

    bool showComplete_ = false;

    bool        isWarning_ = false;
    std::string warningText_ = "";
    int         delay = 0;

    static constexpr int SKIP_REQUIRED = 60;
    int skipHoldFrames_ = 0;

    int autoAdvanceTimer_ = -1;
};