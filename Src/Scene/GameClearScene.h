#pragma once
#include "SceneBase.h"

class GameClearScene :
	public SceneBase
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
};

