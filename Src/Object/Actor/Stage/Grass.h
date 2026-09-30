#pragma once
#include "../ActorBase.h"
#include <vector>

class ColliderModel;

class Grass : public ActorBase
{
public:

	Grass();
	virtual ~Grass();

	void Update() override;
	void Draw() override;

	void InitLoad() override;
	void InitTransform() override;
	void InitCollider() override;
	void InitAnimation() override;
	void InitPost() override;

	void AddGrass(const VECTOR& pos, float scale);

	void AddHitCollider(ColliderModel* collider);

	// 指定範囲にランダムに草を生成（地面にスナップ）
	void GenerateField(int count, float rangeXZ);

	void SetPlayerPos(const VECTOR& pos) { playerPos_ = pos; }

	// モデル制御の基本情報
	Transform drawTransform_;

private:

	// 地面の座標を取得する
	bool GetGroundPosition(VECTOR& pos);

private:

	struct GrassData
	{
		VECTOR pos;
		float scale;
	};

	std::vector<GrassData> grasses_;

	ColliderModel* stageCollider_ = nullptr;

	float time_;

	float windPower_;

	float windSpeed_;

	// プレイヤーが近づいた時に草を押し倒す設定
	VECTOR playerPos_;

	float bendRadius_;   // この距離より近いと押し倒される
	float bendStrength_;  // 押し倒しの強さ
};