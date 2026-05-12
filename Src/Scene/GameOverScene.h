#pragma once
#include "SceneBase.h"

class GameOverScene :
	public SceneBase
{
public:

	GameOverScene();
	~GameOverScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void Release() override;

};

