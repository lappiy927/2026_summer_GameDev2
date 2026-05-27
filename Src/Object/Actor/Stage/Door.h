#pragma once
#include "../../../Object/Actor/ActorBase.h"


class Door : public ActorBase
{
public:

	Door(void);
	~Door(void);

	// 初期化
	void Init(void);

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

	void Open(void);

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	void InitCollider(void) override;

	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

private:

	float slideAmount_ = 0.0f;
	bool isOpen_ = false;

	VECTOR leftBasePos_;
	VECTOR rightBasePos_;

	VECTOR leftPos_;
	VECTOR rightPos_;

	int leftHandle_;
	int rightHandle_;

};

