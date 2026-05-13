#pragma once
#include "../CharactorBase.h"
class Player;

class EnemyBase :
    public CharactorBase
{
public:

    enum class STATE
    {
        IDLE,
        CHASE,
        ATTACK,
        DAMAGE,
        DEAD
    };

    EnemyBase();
    virtual ~EnemyBase();

    virtual void UpdateProcess() override;
    virtual void UpdateProcessPost() override;

    // 敵AI
    virtual void AI() = 0;

    // ターゲット設定
    void SetTarget(Player* player);

    // 死亡確認
    bool IsDead() const;

    void SetPos(const VECTOR& pos);

    bool IsHit(Player* player);

protected:

    // 状態
    STATE state_;

    // プレイヤー
    Player* target_;

    // HP
    int hp_;

    // 死亡フラグ
    bool isDead_;

    // 索敵距離
    float searchRange_;

    // 攻撃距離
    float attackRange_;

    // 敵移動速度
    float moveSpeedEnemy_;

protected:

    // 状態更新
    void UpdateIdle();
    void UpdateChase();
    void UpdateAttack();
    void UpdateDamage();
    void UpdateDead();

    // プレイヤー方向
    VECTOR GetPlayerDirection() const;

    // プレイヤー距離
    float GetPlayerDistance() const;

    // プレイヤー発見
    bool SearchPlayer() const;

    // ダメージ
    void Damage(int power);

    VECTOR knockbackPow_ = AsoUtility::VECTOR_ZERO;
    int knockbackTimer_ = 0;

};