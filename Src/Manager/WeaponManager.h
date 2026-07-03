#include <DxLib.h>

class Player;
class WeaponBase;
class Katana;
class Gun;
class ColliderCapsule;

class WeaponManager
{
public:

    enum class WEAPON_TYPE
    {
        KATANA,
        GUN,
    };

    WeaponManager(Player* player);
    ~WeaponManager(void);

    void Init(void);
    void Update(void);
    void Draw(void);
    void Release(void);

    // アクティブ武器を切り替える
    void SetActiveWeapon(WEAPON_TYPE type);

    // 現在のアクティブ武器タイプを取得
    WEAPON_TYPE GetActiveWeaponType(void) const;

    // アクティブ武器の攻撃コライダーを取得
    ColliderCapsule* GetActiveCollider(void) const;

private:

    // 非アクティブ武器を背中のボーンに固定
    void UpdateHolster(WeaponBase* weapon, const char* boneName, VECTOR localPos, VECTOR rotEuler);

    Player* player_;

    Katana* katana_;
    Gun* gun_;

    WEAPON_TYPE activeType_;

    // 非アクティブ時の収納オフセット定数
    static constexpr VECTOR HOLSTER_KATANA_POS = { -25.0f,-30.0f, 10.0f };
    static constexpr VECTOR HOLSTER_KATANA_ROT = { -DX_PI_F / 4.0f, 0.0f,DX_PI_F };
    static constexpr VECTOR HOLSTER_GUN_POS = { -25.0f, -50.0f, 0.0f };
    static constexpr VECTOR HOLSTER_GUN_ROT = { DX_PI_F,  DX_PI_F / 2.0f,0.0f };

    // 収納先ボーン名
    static constexpr const char* HOLSTER_BONE = "spine";
};
