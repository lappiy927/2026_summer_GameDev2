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

    bool GetGroundPosition(VECTOR& pos);

private:

    struct GrassData
    {
        VECTOR pos;
        float scale;
    };


    std::vector<GrassData> grasses_;

    ColliderModel* stageCollider_ = nullptr;

    float time_ = 0.0f;

    float windPower_ = 20.0f;

    float windSpeed_ = 2.0f;

    // プレイヤーが近づいた時に草を押し倒す設定
    VECTOR playerPos_ = VGet(0.0f, 0.0f, 0.0f);

    float bendRadius_ = 150.0f;   // この距離より近いと押し倒される
    float bendStrength_ = 40.0f;  // 押し倒しの強さ
};