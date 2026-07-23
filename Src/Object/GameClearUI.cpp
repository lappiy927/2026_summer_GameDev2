#include "GameClearUI.h"

// ============================================================
//  ゲームオーバー時の一言コメント
//  セリフはここを編集してください
// ============================================================
static const char* kGameClearMessage_KB =
"ようやった　今日からおまんも、一人前の剣士じゃ\n(Enter、Spaceでタイトルに戻る)";

static const char* kGameClearMessage_Pad =
"ようやった　今日からおまんも、一人前の剣士じゃ\n(Bボタンでタイトルに戻る)";
static const char* kGameClearSpeaker = "殺魔剣士";

// --- カラー定数（TutorialUI と同じ配色）---
static unsigned int GOC_WIN_BG() { return GetColor(8, 4, 20); }
static unsigned int GOC_WIN_BORDER() { return GetColor(140, 80, 220); }
static unsigned int GOC_NAME() { return GetColor(192, 132, 252); }
static unsigned int GOC_TEXT() { return GetColor(240, 232, 255); }
static unsigned int GOC_DIVIDER() { return GetColor(80, 40, 140); }

// ============================================================
GameClearUI::~GameClearUI()
{
    Release();
}

void GameClearUI::Release()
{
    if (charImage_ != -1) { DeleteGraph(charImage_); charImage_ = -1; }
    if (font_ != -1) { DeleteFontToHandle(font_); font_ = -1; }
    RemoveFontResourceEx("Data/Font/玉ねぎ楷書激無料版v7改.ttf", FR_PRIVATE, 0);
}

// ============================================================
void GameClearUI::Init()
{
    // 立ち絵画像（差し替えたい場合はパスを変更してください）
    charImage_ = LoadGraph("Data/Image/Tutorial.png");

    AddFontResourceEx("Data/Font/玉ねぎ楷書激無料版v7改.ttf", FR_PRIVATE, 0);

    font_ = CreateFontToHandle("玉ねぎ楷書激無料版v7改", 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE);

    speakerName_ = kGameClearSpeaker;

    chatW_ = screenW - 80;
    chatH_ = 200;
    chatX_ = 40;
    chatY_ = screenH - chatH_ - 30;

    charX_ = chatX_;
    charY_ = chatY_ - 100;

    // キーボードかパッドかでセリフ（Enter / Bボタン表記）を切り替える
    const char* message = (GetJoypadNum() == 0)
        ? kGameClearMessage_KB
        : kGameClearMessage_Pad;

    StartTypewriter(message);
}

// ============================================================
void GameClearUI::Update()
{
    UpdateTypewriter();
}

void GameClearUI::Draw() const
{
    DrawCharacter();
    DrawChatWindow();
}

// ============================================================
void GameClearUI::StartTypewriter(const std::string& text)
{
    fullText_ = text;
    shownText_ = "";
    typeTimer_ = 0;
    typingDone_ = false;
}

void GameClearUI::UpdateTypewriter()
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
void GameClearUI::DrawCharacter() const
{
    if (charImage_ == -1) return;
    DrawGraph(charX_, charY_, charImage_, TRUE);
}

// ============================================================
void GameClearUI::DrawChatWindow() const
{
    constexpr int PAD = 20;
    constexpr int NAME_H = 30;
    constexpr int LINE_H = 28;
    constexpr int RADIUS = 6;

    // 背景（半透明）
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 210);
    DrawRoundRect(chatX_, chatY_, chatX_ + chatW_, chatY_ + chatH_,
        RADIUS, RADIUS, GOC_WIN_BG(), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 枠線
    DrawRoundRect(chatX_, chatY_, chatX_ + chatW_, chatY_ + chatH_,
        RADIUS, RADIUS, GOC_WIN_BORDER(), FALSE);

    // 話者名
    DrawStringToHandle(chatX_ + PAD, chatY_ + 10,
        speakerName_.c_str(), GOC_NAME(), font_);

    // 区切り線
    DrawLine(chatX_ + PAD, chatY_ + NAME_H + 8,
        chatX_ + chatW_ - PAD, chatY_ + NAME_H + 8,
        GOC_DIVIDER());

    // セリフ（タイプライター）
    int textY = chatY_ + NAME_H + 16;
    DrawStringToHandle(chatX_ + PAD, textY,
        shownText_.c_str(), GOC_TEXT(), font_);

    // タイプ中カーソル点滅
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
                "■", GOC_NAME(), font_);
        }
    }
}