#pragma once
#include <DxLib.h>
#include <map>
#include <string>

class SoundManager
{
public:

	enum class SRC
	{
		Attack,
		Walk,
		Run,
		Battle,
		EnemyDai,
	};

	static void CreateInstance(void);
	static SoundManager& GetInstance(void);

	void Init(void);
	void Release(void);
	void Destroy(void);

	void Play(SRC src, bool loop = false, int volume = 255);
	void Pause(SRC src);
	void Resume(SRC src);
	void Stop(SRC src);
	void StopAll(void);
	void SetVolume(SRC src, int volume);
	void SetLoop(SRC src, bool loop);
	bool IsPlaying(SRC src);

private:

	struct SoundData
	{
		int  handle = -1;
		bool isLooping = false;
	};

	static SoundManager* instance_;

	// 登録テーブル (パスのみ保持)
	std::map<SRC, std::string> pathMap_;

	// ロード済みサウンド
	std::map<SRC, SoundData>   soundMap_;

	// 未ロードなら読み込んで返す
	SoundData* _Load(SRC src);

	SoundManager(void);
	~SoundManager(void) = default;
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
};
