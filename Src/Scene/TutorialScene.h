#pragma once
#include "SceneBase.h"

class AnimationController;
class Player;
class Katana;
class Stage;
class EnemyMob;
class TutorialUI;

class TutorialScene : public SceneBase
{
public:

	// コンストラクタ
	TutorialScene(void);

	// デストラクタ
	~TutorialScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	Player* player_;
	Katana* katana_;
	Stage* stage_;
	EnemyMob* enemy_;

	TutorialUI* tutorialUI_;

	static constexpr int WALK_REQUIRED = 120;
	static constexpr int RUN_REQUIRED = 120;

	// --- 入力検知用 ---
	void CheckTutorialInput();
	int  walkFrames_ = 0;
	int  runFrames_ = 0;
	bool prevMouseLeft_ = false;

	// --- 行動範囲制限 ---
	void  CheckAreaLimit();             // 毎フレーム呼ぶ
	bool  IsOutOfArea() const;          // 範囲外判定

	VECTOR areaCenter_ = { 0.0f, 0.0f, 0.0f };   // 範囲の中心（Init で設定）
	float  areaRadius_ = 3000.0f;
};