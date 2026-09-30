#pragma once
#include "../../Common/Quaternion.h"
#include "../../Utility/AsoUtility.h"

/// <summary>
/// モデル制御の基本情報
/// 大きさ：VECTOR基準
/// 回転　：Quaternion基準
/// 位置　：VECTOR基準
/// </summary>

class Transform
{
public:

	// モデルのハンドルID
	int modelId_;

	// 大きさ
	VECTOR scl_;

	// 回転
	VECTOR rot_;

	// 位置
	VECTOR pos_;
	VECTOR localPos_;

	// 行列
	MATRIX matScl_;
	MATRIX matRot_;
	MATRIX matPos_;

	// 回転
	Quaternion quaRot_;

	// ローカル回転
	Quaternion quaRotLocal_;

	// コンストラクタ
	Transform(void);

	// デストラクタ
	virtual ~Transform(void);

	// モデル制御の基本情報更新
	void Update(void);

	// 解放
	void Release(void);

	// モデルのハンドルIDを設定
	void SetModel(int id);

	// 前方方向を取得
	VECTOR GetForward(void) const;

	// 後方方向を取得
	VECTOR GetBack(void) const;

	// 右方向を取得
	VECTOR GetRight(void) const;

	// 左方向を取得
	VECTOR GetLeft(void) const;

	// 上方向を取得
	VECTOR GetUp(void) const;

	// 下方向を取得
	VECTOR GetDown(void) const;

	// 対象方向を取得
	VECTOR GetDir(const VECTOR& dir) const;

	// ワールド行列を取得
	MATRIX GetWorldMatrix()const;
};