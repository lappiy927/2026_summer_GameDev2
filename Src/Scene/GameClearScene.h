#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/AnimationController.h"
#include "../Object/GameClearUI.h"  

class GameClearScene : public SceneBase
{
public:

	GameClearScene();
	~GameClearScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;

private:

	int fontHandle;

	Transform charactor_;
	AnimationController* animationController_ = nullptr;

	int samuraiModelId = -1;
	float timer_ = 0.0f;
	float fallDuration_ = 1.2f;
	float textDelay_ = 0.9f;
	bool showText_ = false;

	float textAnimTimer_ = 0.0f;
	float textAnimDuration_ = 1.2f;
	int textStartY_ = 100;
	int textEndY_ = 300;
	int currentTextY_ = 300;
	int currentTextAlpha_ = 255;

	GameClearUI gameClearUI_;  // Å•í«â¡
};