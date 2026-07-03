#pragma once
#include <DxLib.h>
#include <string>

class GameClearUI
{
public:
    GameClearUI() = default;
    ~GameClearUI();

    void Init();
    void Update();
    void Draw() const;
    void Release();

    int screenW = 1280;
    int screenH = 720;

private:
    void StartTypewriter(const std::string& text);
    void UpdateTypewriter();

    void DrawCharacter()  const;
    void DrawChatWindow() const;

    int charImage_ = -1;
    int font_ = -1;

    std::string speakerName_ = "ŽE–‚Œ•Žm";
    std::string fullText_;
    std::string shownText_;
    int  typeTimer_ = 0;
    int  typeInterval_ = 3;
    bool typingDone_ = false;

    int chatX_ = 0, chatY_ = 0, chatW_ = 0, chatH_ = 0;
    int charX_ = 0, charY_ = 0;
};