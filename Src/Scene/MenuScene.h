#pragma once
#include "SceneBase.h"

class MenuScene : public SceneBase
{
public:

	// コンストラクタ
	MenuScene(void);

	// デストラクタ
	~MenuScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	int menuBg_;

	int buttonPlay_;
	int buttonPlaySelect_;

	int buttonOption_;
	int buttonOptionSelect_;

	int buttonExit_;
	int buttonExitSelect_;

	int selectNo_ = 0;


};

