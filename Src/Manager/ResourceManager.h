#pragma once
#include <map>
#include <string>
#include "Resource.h"

class ResourceManager
{

public:

	// リソース名
	enum class SRC
	{
		TITLE,
		PUSH_SPACE,
		PLAYER,
		PLAYER_IDLE,
		PLAYER_RUN,
		PLAYER_FAST_RUN,
		MAIN_STAGE,
		PLAYER_SHADOW,
		KATANA,
	};

	// 明示的にインスタンスを生成する
	static void CreateInstance(void);

	// 静的インスタンスの取得
	static ResourceManager& GetInstance(void);

	// 初期化
	void Init(void);

	// 解放(シーン切替時に一旦解放)
	void Release(void);

	// リソースの完全破棄
	void Destroy(void);

	// リソースのロード
	const Resource& Load(SRC src);

	// リソースの複製ロード(モデル用）
	int LoadModelDulicate(SRC src);

private:

	// 静的インスタンス
	static ResourceManager* instance_;

	// リソース管理の対象
	std::map<SRC, Resource*> resourcesMap_;

	// 読み込み済みリソース
	std::map<SRC, Resource&> loadedMap_;

	Resource dummy_;

	// デフォルトコンストラクタをprivateにして、
	// 外部から生成できない用にする
	ResourceManager(void);
	~ResourceManager(void) = default;

	// コピー禁止
	ResourceManager(const ResourceManager& manager) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	// 内部ロード
	Resource& _Load(SRC src);
};

