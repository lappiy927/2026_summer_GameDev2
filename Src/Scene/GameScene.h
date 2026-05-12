#pragma once
#include <vector>
#include <memory>

#include "SceneBase.h"

class Stage;
class Player;
class EnemyBase;

class GameScene : public SceneBase
{

public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	Stage* stage_;
	Player* player_;

	std::vector<std::shared_ptr<EnemyBase>> enemies_;

	float limitTime_;

	bool hit_ = false;
};