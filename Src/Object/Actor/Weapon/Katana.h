#pragma once
#include "WeaponBase.h"
#include "../../Collider/ColliderCapsule.h"

class Katana : public WeaponBase
{
public:

    //コンストラクタ
    Katana(Player* player);

    //デストラクタ
    ~Katana(void);

    void Init(void)override;
    void Update(void)override;

    ColliderCapsule* GetCollider() const;

private:

    ColliderCapsule* attackCollider_ = nullptr;

    bool isAttack_ = false;

    float attackTimer_ = 0.0f;

    bool oldMouse_ = false;

};