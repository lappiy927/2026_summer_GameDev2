#pragma once
#include <vector>
#include <memory>

#include "SceneBase.h"

class Stage;
class Player;
<<<<<<< HEAD
class Katana;
=======
class EnemyBase;
>>>>>>> b141a3e7c223090454105bdc85fa58d72234d14e

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
	Katana* katana_;

	std::vector<std::shared_ptr<EnemyBase>> enemies_;

	float limitTime_;

	bool hit_ = false;
};