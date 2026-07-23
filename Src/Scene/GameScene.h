#pragma once
#include <vector>
#include <memory>

#include "SceneBase.h"

class Stage;
class Player;
class WeaponManager;
class Door;
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
	WeaponManager* weaponMng_;
	Door* door_;


	std::vector<std::shared_ptr<EnemyBase>> enemies_;

	int lifeHandle_;

	int daiHandle_;

	int timeUI_;


	int slashEffectHandle_;

	float limitTime_;

	bool hit_ = false;

	bool isBossRoomOpen_ = false;

	int bossRoomOpenTimer_ = 0;

	bool isGameStart_ = false;
	float countDown_ = 4.0f;

	int countImage_[4];

	int remainEnemy_ = 10;
	int remainEnemyTimer_ = 0;


};