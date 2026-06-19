#include "TutorialUI.h"
#include "../Manager/InputManager.h"

// ============================================================
//  ステップ定義テーブル
//  セリフ・ヒントはここを編集してください
// ============================================================
static const std::vector<TutorialStepData> kStepTable =
{
    {
        TutorialStep::WALK,
        "殺魔剣士",
        "まずは移動を覚えもんそ！\nWASD キーを押して歩いてみたもんせ。",
        "[ WASD ] 歩く"
    },
    {
        TutorialStep::RUN,
        "殺魔剣士",
        "次は走りじゃ！\nShift を押しながら WASD で素早よ移動できっ。",
        "[ Shift + WASD ] 走る"
    },
    {
        TutorialStep::ATTACK,
        "殺魔剣士",
        "後は攻撃！\n敵を一体配置したで倒してみぃ。",
        "[ 左クリック ] 攻撃"
    },
};

static const std::vector<TutorialStepData> pStepTable =
{
    {
        TutorialStep::WALK,
        "殺魔剣士",
        "まずは移動を覚えもんそ！\n左スティックを動かして歩いてみたもんせ。",
        "[ 左スティック ] 歩く"
    },
    {
        TutorialStep::RUN,
        "殺魔剣士",
        "次は走りじゃ！\nLT を押しながら 左スティック で素早よ移動できっ。",
        "[ LT + 左スティック ] 走る"
    },
    {
        TutorialStep::ATTACK,
        "殺魔剣士",
        "後は攻撃！\n敵を一体配置したで倒してみぃ。",
        "[RT ] 攻撃"
    },
};

// --- カラー定数（GetColor は関数なので inline 関数で定義）---
static unsigned int COL_WIN_BG() { return GetColor(8, 4, 20); }
static unsigned int COL_WIN_BORDER() { return GetColor(140, 80, 220); }
static unsigned int COL_NAME() { return GetColor(192, 132, 252); }
static unsigned int COL_TEXT() { return GetColor(240, 232, 255); }
static unsigned int COL_HINT() { return GetColor(160, 120, 200); }
static unsigned int COL_DIVIDER() { return GetColor(80, 40, 140); }
static unsigned int COL_DOT_DONE() { return GetColor(140, 80, 220); }
static unsigned int COL_DOT_NOW() { return GetColor(192, 132, 252); }
static unsigned int COL_DOT_NONE() { return GetColor(60, 30, 100); }
static unsigned int COL_COMPLETE() { return GetColor(220, 180, 255); }

// ============================================================
TutorialUI::~TutorialUI()
{
}

void TutorialUI::Release()
{
    if (charImage_ != -1) { DeleteGraph(charImage_); charImage_ = -1; }
    if (font_ != -1) { DeleteFontToHandle(font_); font_ = -1; }
}

// ============================================================
void TutorialUI::Init()
{
    charImage_ = LoadGraph("Data/Image/Tutorial.png");

    font_ = CreateFontToHandle("ＭＳ ゴシック", 20, 3,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    steps_ = (GetJoypadNum() == 0) ? kStepTable : pStepTable;
    stepIndex_ = 0;
    currentStep_ = steps_[0].step;

    chatW_ = screenW - 80;
    chatH_ = 200;
    chatX_ = 40;
    chatY_ = screenH - chatH_ - 30;

    charX_ = chatX_;
    charY_ = chatY_ - 100;

    StartTypewriter(steps_[0].message);
}

// ============================================================
void TutorialUI::Update()
{
    if (currentStep_ == TutorialStep::COMPLETE) return;

    // ダイアログ中はタイプライターなどを止める
    if (isSkipDialogOpen_)
    {
        UpdateSkipDialog();
        return;
    }

    UpdateTypewriter();

    // 警告セリフが終わったら元のセリフに戻す
    if (isWarning_ && typingDone_)
    {
        delay++;
        if (delay >= 60)
        {
            isWarning_ = false;
            StartTypewriter(steps_[stepIndex_].message);
            delay = 0;
            charImage_ = LoadGraph("Data/Image/Tutorial.png");
        }
    }
}

void TutorialUI::Draw() const
{
    DrawCharacter();
    DrawChatWindow();
    if (showComplete_)     DrawCompleteOverlay();
    if (isSkipDialogOpen_) DrawSkipDialog();
}

// ============================================================
void TutorialUI::NotifyWalkSuccess()
{
    if (currentStep_ == TutorialStep::WALK)   AdvanceStep();
}
void TutorialUI::NotifyRunSuccess()
{
    if (currentStep_ == TutorialStep::RUN)    AdvanceStep();
}
void TutorialUI::NotifyAttackSuccess()
{
    if (currentStep_ == TutorialStep::ATTACK) AdvanceStep();
}

void TutorialUI::ShowWarning(const std::string& message)
{
    if (isWarning_) return;
    isWarning_ = true;
    warningText_ = message;
    StartTypewriter(message);
    charImage_ = LoadGraph("Data/Image/Tutorial2.png");
}

// ============================================================
void TutorialUI::OpenSkipDialog()
{
    if (isSkipDialogOpen_) return;
    isSkipDialogOpen_ = true;
    isSkipConfirmed_ = false;
    dialogCursorPos_ = 1;  // デフォルトは「いいえ」
}

void TutorialUI::UpdateSkipDialog()
{
    auto& ins = InputManager::GetInstance();

    InputManager::JOYPAD_IN_STATE padState =
        ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

    // ← でカーソルを「はい」へ
    bool nowLeft = (CheckHitKey(KEY_INPUT_LEFT) != 0 ||
        ins.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::DPAD_LEFT) != 0);
    if (nowLeft && !prevLeft_) dialogCursorPos_ = 0;
    prevLeft_ = nowLeft;

    // → でカーソルを「いいえ」へ
    bool nowRight = (CheckHitKey(KEY_INPUT_RIGHT) != 0 ||
        ins.IsPadBtnTrgDown(
            InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::DPAD_RIGHT) != 0);
    if (nowRight && !prevRight_) dialogCursorPos_ = 1;
    prevRight_ = nowRight;

    // Enter で決定
    bool nowEnter = (CheckHitKey(KEY_INPUT_RETURN) != 0 ||
        ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
            InputManager::JOYPAD_BTN::RIGHT) != 0);
    if (nowEnter && !prevEnter_)
    {
        if (dialogCursorPos_ == 0)
        {
            // 「はい」→ スキップ確定
            isSkipConfirmed_ = true;
            isSkipDialogOpen_ = false;
        }
        else
        {
            // 「いいえ」→ ダイアログを閉じるだけ
            isSkipDialogOpen_ = false;
        }
    }
    prevEnter_ = nowEnter;
}

// ============================================================
void TutorialUI::DrawSkipDialog() const
{
    constexpr int DLG_W = 420;
    constexpr int DLG_H = 160;
    constexpr int RADIUS = 8;
    constexpr int BTN_W = 120;
    constexpr int BTN_H = 40;

    int dx = screenW / 2 - DLG_W / 2;
    int dy = chatY_ - DLG_H - 20;
    int btnY = dy + DLG_H - BTN_H - 20;

    // 背景（半透明）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
    DrawRoundRect(dx, dy, dx + DLG_W, dy + DLG_H,
        RADIUS, RADIUS, GetColor(8, 4, 20), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 枠線
    DrawRoundRect(dx, dy, dx + DLG_W, dy + DLG_H,
        RADIUS, RADIUS, COL_WIN_BORDER(), FALSE);

    // メッセージ
    DrawStringToHandle(dx + 30, dy + 28,
        "チュートリアルをスキップしますか？",
        COL_TEXT(), font_);

    // ---- 「はい」ボタン ----
    int yesX = dx + DLG_W / 2 - BTN_W - 16;
    unsigned int yesBg = (dialogCursorPos_ == 0) ? GetColor(140, 60, 200) : GetColor(30, 15, 60);
    unsigned int yesBdr = (dialogCursorPos_ == 0) ? GetColor(200, 140, 255) : GetColor(80, 40, 140);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawBox(yesX, btnY, yesX + BTN_W, btnY + BTN_H, yesBg, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawBox(yesX, btnY, yesX + BTN_W, btnY + BTN_H, yesBdr, FALSE);
    DrawStringToHandle(yesX + 36, btnY + 10, "はい", COL_TEXT(), font_);

    // ---- 「いいえ」ボタン ----
    int noX = dx + DLG_W / 2 + 16;
    unsigned int noBg = (dialogCursorPos_ == 1) ? GetColor(140, 60, 200) : GetColor(30, 15, 60);
    unsigned int noBdr = (dialogCursorPos_ == 1) ? GetColor(200, 140, 255) : GetColor(80, 40, 140);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
    DrawBox(noX, btnY, noX + BTN_W, btnY + BTN_H, noBg, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawBox(noX, btnY, noX + BTN_W, btnY + BTN_H, noBdr, FALSE);
    DrawStringToHandle(noX + 24, btnY + 10, "いいえ", COL_TEXT(), font_);

    // 操作ヒント
    DrawStringToHandle(dx + 30, dy + DLG_H - 22,
        "← → で選択    Enter で決定",
        COL_HINT(), font_);
}

// ============================================================
void TutorialUI::AdvanceStep()
{
    stepIndex_++;
    if (stepIndex_ >= static_cast<int>(steps_.size()))
    {
        currentStep_ = TutorialStep::COMPLETE;
        showComplete_ = true;
        return;
    }
    currentStep_ = steps_[stepIndex_].step;
    StartTypewriter(steps_[stepIndex_].message);
}

void TutorialUI::StartTypewriter(const std::string& text)
{
    fullText_ = text;
    shownText_ = "";
    typeTimer_ = 0;
    typingDone_ = false;
}

void TutorialUI::UpdateTypewriter()
{
    if (typingDone_) return;

    typeTimer_++;
    if (typeTimer_ >= typeInterval_)
    {
        typeTimer_ = 0;
        size_t pos = shownText_.size();
        if (pos < fullText_.size())
        {
            unsigned char c = static_cast<unsigned char>(fullText_[pos]);
            int charBytes = ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC)) ? 2 : 1;

            if (pos + charBytes <= fullText_.size())
                shownText_ += fullText_.substr(pos, charBytes);
            else
                shownText_ += fullText_[pos];
        }
        else
        {
            typingDone_ = true;
        }
    }
}

// ============================================================
void TutorialUI::DrawCharacter() const
{
    if (charImage_ == -1) return;
    DrawGraph(charX_, charY_, charImage_, TRUE);
}

// ============================================================
void TutorialUI::DrawChatWindow() const
{
    constexpr int PAD = 20;
    constexpr int NAME_H = 30;
    constexpr int HINT_H = 24;
    constexpr int LINE_H = 28;
    constexpr int RADIUS = 6;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210);
    DrawRoundRect(chatX_, chatY_, chatX_ + chatW_, chatY_ + chatH_,
        RADIUS, RADIUS, COL_WIN_BG(), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawRoundRect(chatX_, chatY_, chatX_ + chatW_, chatY_ + chatH_,
        RADIUS, RADIUS, COL_WIN_BORDER(), FALSE);

    if (stepIndex_ < static_cast<int>(steps_.size()))
    {
        DrawStringToHandle(chatX_ + PAD, chatY_ + 10,
            steps_[stepIndex_].speakerName.c_str(),
            COL_NAME(), font_);
    }

    DrawLine(chatX_ + PAD, chatY_ + NAME_H + 8,
        chatX_ + chatW_ - PAD, chatY_ + NAME_H + 8,
        COL_DIVIDER());

    int textY = chatY_ + NAME_H + 16;
    DrawStringToHandle(chatX_ + PAD, textY, shownText_.c_str(), COL_TEXT(), font_);

    if (!typingDone_)
    {
        bool blink = ((GetNowCount() / 400) % 2 == 0);
        if (blink)
        {
            int lineCount = 0;
            for (char c : shownText_) if (c == '\n') lineCount++;

            size_t lastNl = shownText_.rfind('\n');
            std::string lastLine = (lastNl == std::string::npos)
                ? shownText_
                : shownText_.substr(lastNl + 1);
            int tw = GetDrawStringWidthToHandle(
                lastLine.c_str(), static_cast<int>(lastLine.size()), font_);

            DrawStringToHandle(chatX_ + PAD + tw,
                textY + lineCount * LINE_H,
                "■", COL_NAME(), font_);
        }
    }

    DrawProgressDots();

    if (stepIndex_ < static_cast<int>(steps_.size()))
    {
        DrawStringToHandle(chatX_ + PAD,
            chatY_ + chatH_ - HINT_H - 8,
            steps_[stepIndex_].hintText.c_str(),
            COL_HINT(), font_);
    }
}

// ============================================================
void TutorialUI::DrawProgressDots() const
{
    constexpr int DOT_W = 28;
    constexpr int DOT_H = 4;
    constexpr int DOT_GAP = 6;

    int total = static_cast<int>(steps_.size());
    int totalWidth = total * DOT_W + (total - 1) * DOT_GAP;
    int startX = chatX_ + chatW_ - totalWidth - 20;
    int dotY = chatY_ + chatH_ - 14;

    for (int i = 0; i < total; i++)
    {
        int dx = startX + i * (DOT_W + DOT_GAP);
        unsigned int col = (i < stepIndex_) ? COL_DOT_DONE() :
            (i == stepIndex_) ? COL_DOT_NOW() :
            COL_DOT_NONE();
        DrawBox(dx, dotY, dx + DOT_W, dotY + DOT_H, col, TRUE);
    }
}

// ============================================================
void TutorialUI::DrawCompleteOverlay() const
{
    int ox = screenW / 2 - 200;
    int oy = screenH / 2 - 80;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 215);
    DrawBox(ox, oy, ox + 400, oy + 160, GetColor(10, 5, 30), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawBox(ox, oy, ox + 400, oy + 160, COL_WIN_BORDER(), FALSE);

    DrawStringToHandle(ox + 110, oy + 40,
        "チュートリアル完了！", COL_COMPLETE(), font_);

    if (GetJoypadNum() == 0)
    {
        DrawStringToHandle(ox + 80, oy + 90, "エンターキーで次へ", COL_HINT(), font_);
    }
    else
    {
        DrawStringToHandle(ox + 80, oy + 90, "Bボタンで次へ", COL_HINT(), font_);
    }
}