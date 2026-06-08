#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class AnimationController;

class TitleScene : public SceneBase
{

public:

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	//タイトル画像のX,Y座標
	static constexpr int IMG_TITLE_POS_Y = 250;
	static constexpr int IMG_PUSH_POS_Y = 500;

	//球体の大きさ
	static constexpr float SCL_SMALL_PLANET = 0.7f;

	//キャラクターの座標
	static constexpr VECTOR POS_SMALL_PLANET = { -250.0f, -100.0f, -100.0f };

	//キャラクターの大きさ
	static constexpr float SCL_CHARACTOR = 0.4f;

	//キャラクターの大きさ
	static constexpr VECTOR ROT_CHARACTOR = { 0.0f,-90.0f * DX_PI_F / 180.0f,0.0f };
	static constexpr VECTOR ROT_ROCAL_CHARACTOR = { 0.0f,180.0f * DX_PI_F / 180.0f,0.0f };

	//キャラクターの座標
	static constexpr VECTOR POS_CHARACTOR = { -250.0f, -32.0f, -105.0f };
	//画像ハンドル
	int imgTitle_;
	int imgPushSpace_;

	Transform charactor_;

	Transform enemy_;

	//アニメーション
	AnimationController* animationController_;

	int selectNo_ = 0;

	int buttonPlay_;
	int buttonPlaySelect_;

	int buttonExit_;
	int buttonExitSelect_;
};