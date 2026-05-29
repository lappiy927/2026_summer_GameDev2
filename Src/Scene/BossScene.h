#pragma once
#include <vector>
#include <memory>

#include "SceneBase.h"

class BossStage;
class Player;
class Katana;

class BossScene :
    public SceneBase
{
public:

	// コンストラクタ
	BossScene(void);

	// デストラクタ
	~BossScene(void);

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	BossStage* bossStage_;
	Player* player_;
	Katana* katana_;

};

