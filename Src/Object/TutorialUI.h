#pragma once
#include <DxLib.h>
#include <string>
#include <vector>

// チュートリアルのステップ
enum class TutorialStep
{
    WALK,
    RUN,
    ATTACK,
    COMPLETE,
};

// 1ステップ分のデータ
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

    TutorialStep GetCurrentStep() const { return currentStep_; }
    bool IsFinished()     const { return currentStep_ == TutorialStep::COMPLETE; }
    bool IsWarning()      const { return isWarning_; }

    // 警告セリフを表示する
    void ShowWarning(const std::string& message);

    // スキップダイアログを開く
    void OpenSkipDialog();
    bool IsSkipDialogOpen() const { return isSkipDialogOpen_; }
    bool IsSkipConfirmed()  const { return isSkipConfirmed_; }

    int screenW = 1280;
    int screenH = 720;

private:
    void AdvanceStep();
    void StartTypewriter(const std::string& text);
    void UpdateTypewriter();
    void UpdateSkipDialog();

    void DrawCharacter()       const;
    void DrawChatWindow()      const;
    void DrawProgressDots()    const;
    void DrawCompleteOverlay() const;
    void DrawSkipDialog()      const;

    int charImage_ = -1;
    int font_ = -1;

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

    // --- 警告状態 ---
    bool        isWarning_ = false;
    std::string warningText_ = "";
    int         delay = 0;

    // --- スキップダイアログ ---
    bool isSkipDialogOpen_ = false; // ダイアログ表示中
    bool isSkipConfirmed_ = false; // スキップ確定フラグ
    int  dialogCursorPos_ = 1;

    // カーソル移動の連続入力防止
    bool prevLeft_ = false;
    bool prevRight_ = false;
    bool prevEnter_ = false;
};