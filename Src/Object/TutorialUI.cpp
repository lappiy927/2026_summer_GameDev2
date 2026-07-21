#include "TutorialUI.h"

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
    {
        TutorialStep::WEAPON_CHANGE,
        "殺魔剣士",
        "最後じゃ！\nEキーで武器を切り替えてみぃ。",
        "[ E ] 武器切り替え"
    },
    {
        TutorialStep::GUN_INFO_1,
        "殺魔剣士",
        "銃はQキーで弾を込められっど。",
        ""
    },
    {
        TutorialStep::GUN_INFO_2,
        "殺魔剣士",
        "ただし、5発までしか撃てん。使いどころには気をつけ。",
        ""
    },
    {
        TutorialStep::ENDING,
        "殺魔剣士",
        "よくやったのぅ！\nこれでチュートリアルは終わりじゃ。\n自由に動いてみぃ。",
        "[ エンター ] 次へ"
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
        "[ RT ] 攻撃"
    },
    {
        TutorialStep::WEAPON_CHANGE,
        "殺魔剣士",
        "最後じゃ！\n十字キー左右で武器を切り替えてみぃ。",
        "[ 十字キー左右 ] 武器切り替え"
    },
    {
        TutorialStep::GUN_INFO_1,
        "殺魔剣士",
        "銃は十字キー左で弾を込められっど。",
        ""
    },
    {
        TutorialStep::GUN_INFO_2,
        "殺魔剣士",
        "ただし、5発までしか撃てん。使いどころには気をつけ。",
        ""
    },
    {
        TutorialStep::ENDING,
        "殺魔剣士",
        "よくやったのぅ！\nこれでチュートリアルは終わりじゃ。\n自由に動いてみぃ。",
        "[ B ] 次へ"
    },
};

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

TutorialUI::~TutorialUI() {}

void TutorialUI::Release()
{
    if (charImage_ != -1) { DeleteGraph(charImage_);        charImage_ = -1; }
    if (font_ != -1) { DeleteFontToHandle(font_);      font_ = -1; }
    if (fontSmall_ != -1) { DeleteFontToHandle(fontSmall_); fontSmall_ = -1; }
    RemoveFontResourceEx("Data/Font/玉ねぎ楷書激無料版v7改.ttf", FR_PRIVATE, 0);
}

void TutorialUI::Init()
{
    AddFontResourceEx("Data/Font/玉ねぎ楷書激無料版v7改.ttf", FR_PRIVATE, 0);

    charImage_ = LoadGraph("Data/Image/Tutorial.png");
    font_ = CreateFontToHandle("玉ねぎ楷書激無料版v7改", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
    fontSmall_ = CreateFontToHandle("玉ねぎ楷書激無料版v7改", 13, 2, DX_FONTTYPE_ANTIALIASING_EDGE);

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

void TutorialUI::Update()
{
    if (currentStep_ == TutorialStep::COMPLETE) return;

    UpdateTypewriter();

    // GUN_INFO：全文表示から1秒後に自動で次へ
    if (currentStep_ == TutorialStep::GUN_INFO_1 ||
        currentStep_ == TutorialStep::GUN_INFO_2)
    {
        if (typingDone_)
        {
            if (autoAdvanceTimer_ < 0)
                autoAdvanceTimer_ = 60;

            autoAdvanceTimer_--;
            if (autoAdvanceTimer_ <= 0)
            {
                autoAdvanceTimer_ = -1;
                AdvanceStep();
            }
        }
        return;
    }

    if (isWarning_ && typingDone_ && currentStep_ != TutorialStep::ENDING)
    {
        delay++;
        if (delay >= 60)
        {
            isWarning_ = false;

            // コールバックがあれば呼んでクリア
            if (warningOnFinished_)
            {
                auto cb = warningOnFinished_;
                warningOnFinished_ = nullptr;
                cb();
                return;
            }

            StartTypewriter(steps_[stepIndex_].message);
            delay = 0;
            charImage_ = LoadGraph("Data/Image/Tutorial.png");
        }
    }
}

bool TutorialUI::UpdateSkipHold(bool holding)
{
    if (holding)
    {
        skipHoldFrames_++;
        if (skipHoldFrames_ >= SKIP_REQUIRED)
        {
            skipHoldFrames_ = 0;
            return true;
        }
    }
    else
    {
        skipHoldFrames_ = 0;
    }
    return false;
}

void TutorialUI::Draw() const
{
    DrawCharacter();
    DrawChatWindow();
    if (currentStep_ != TutorialStep::ENDING &&
        currentStep_ != TutorialStep::COMPLETE)
    {
        DrawSkipGauge();
    }
}

void TutorialUI::NotifyWalkSuccess()
{
    if (currentStep_ == TutorialStep::WALK)          AdvanceStep();
}
void TutorialUI::NotifyRunSuccess()
{
    if (currentStep_ == TutorialStep::RUN)           AdvanceStep();
}
void TutorialUI::NotifyAttackSuccess()
{
    if (currentStep_ == TutorialStep::ATTACK)        AdvanceStep();
}
void TutorialUI::NotifyWeaponChangeSuccess()
{
    if (currentStep_ == TutorialStep::WEAPON_CHANGE) AdvanceStep();
}
void TutorialUI::NotifyGunInfoNext()
{
    if (currentStep_ == TutorialStep::GUN_INFO_1 ||
        currentStep_ == TutorialStep::GUN_INFO_2)
    {
        AdvanceStep();
    }
}

void TutorialUI::ShowWarning(const std::string& message,
    std::function<void()> onFinished)
{
    if (isWarning_) return;
    isWarning_ = true;
    warningText_ = message;
    warningOnFinished_ = onFinished;
    StartTypewriter(message);
    charImage_ = LoadGraph("Data/Image/Tutorial2.png");
}

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
    autoAdvanceTimer_ = -1;
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

void TutorialUI::DrawCharacter() const
{
    if (charImage_ == -1) return;
    DrawGraph(charX_, charY_, charImage_, TRUE);
}

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
        DrawStringToHandle(ox + 80, oy + 90, "スペースキーで次へ", COL_HINT(), font_);
    else
        DrawStringToHandle(ox + 80, oy + 90, "Bボタンで次へ", COL_HINT(), font_);
}

void TutorialUI::DrawSkipGauge() const
{
    constexpr int ICON_W = 72;
    constexpr int ICON_H = 30;
    constexpr int RADIUS = 6;
    constexpr int MARGIN = 20;

    int ix = screenW - ICON_W - MARGIN;
    int iy = screenH - ICON_H - MARGIN;

    float rate = static_cast<float>(skipHoldFrames_) /
        static_cast<float>(SKIP_REQUIRED);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
    DrawRoundRect(ix, iy, ix + ICON_W, iy + ICON_H,
        RADIUS, RADIUS, GetColor(20, 10, 40), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawRoundRect(ix, iy, ix + ICON_W, iy + ICON_H,
        RADIUS, RADIUS, GetColor(100, 60, 180), FALSE);

    if (skipHoldFrames_ > 0)
    {
        int fillW = static_cast<int>(ICON_W * rate);
        SetDrawArea(ix, iy, ix + fillW, iy + ICON_H);
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
        DrawRoundRect(ix, iy, ix + ICON_W, iy + ICON_H,
            RADIUS, RADIUS, GetColor(160, 80, 255), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        SetDrawAreaFull();
    }

    std::string btnLabel = (GetJoypadNum() > 0) ? "START" : "ESC";
    int tw = GetDrawStringWidthToHandle(
        btnLabel.c_str(), static_cast<int>(btnLabel.size()), font_);
    DrawStringToHandle(ix + (ICON_W - tw) / 2, iy + (ICON_H - 20) / 2,
        btnLabel.c_str(),
        (skipHoldFrames_ > 0) ? GetColor(255, 255, 255)
        : GetColor(160, 130, 200),
        font_);

    const char* label = "長押しでスキップ";
    int lw = GetDrawStringWidthToHandle(label, static_cast<int>(strlen(label)), fontSmall_);
    DrawStringToHandle(ix + (ICON_W - lw) / 2, iy - 18,
        label, GetColor(140, 110, 180), fontSmall_);
}