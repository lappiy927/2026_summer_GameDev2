#pragma once
#include "WeaponBase.h"

class Katana : public WeaponBase
{
public:

    //コンストラクタ
    Katana(Player* player);

    //デストラクタ
    ~Katana(void);

    void Init(void)override;
    void Update(void)override;

};