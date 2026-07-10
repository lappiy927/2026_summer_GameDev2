#include "SoundManager.h"
#include "../Application.h"
#include <cassert>

SoundManager* SoundManager::instance_ = nullptr;

void SoundManager::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new SoundManager();
	}
	instance_->Init();
}

SoundManager& SoundManager::GetInstance(void)
{
	return *instance_;
}

SoundManager::SoundManager(void)
{
}


void SoundManager::Init(void)
{
	static std::string PATH_SND = "Data/Sound/";

	// BGM
	pathMap_.emplace(SRC::Battle, PATH_SND + "Battle.mp3");

	// SE
	pathMap_.emplace(SRC::Attack, PATH_SND + "Attack.mp3");
	pathMap_.emplace(SRC::Walk, PATH_SND + "Walk.mp3");
	pathMap_.emplace(SRC::Run, PATH_SND + "Run.mp3");
	pathMap_.emplace(SRC::EnemyDai, PATH_SND + "devil_scared1.mp3");
	pathMap_.emplace(SRC::Shot,PATH_SND+"shot.mp3")


}

void SoundManager::Release(void)
{
	for (auto& [src, data] : soundMap_)
	{
		if (data.handle != -1)
		{
			StopSoundMem(data.handle);
			DeleteSoundMem(data.handle);
		}
	}
	soundMap_.clear();
}

void SoundManager::Destroy(void)
{
	Release();
	pathMap_.clear();
	delete instance_;
	instance_ = nullptr;
}

//=============================================================
// 内部ロード
//=============================================================

SoundManager::SoundData* SoundManager::_Load(SRC src)
{
	// ロード済みチェック
	auto lIt = soundMap_.find(src);
	if (lIt != soundMap_.end())
	{
		return &lIt->second;
	}

	// 登録チェック
	auto pIt = pathMap_.find(src);
	if (pIt == pathMap_.end())
	{
		return nullptr;
	}

	// ロード
	int handle = LoadSoundMem(pIt->second.c_str());

	soundMap_.emplace(src, SoundData{ handle, false });
	return &soundMap_[src];
}

//=============================================================
// 再生 / 停止 / 一時停止
//=============================================================

void SoundManager::Play(SRC src, bool loop, int volume)
{
	SoundData* data = _Load(src);
	if (!data) { return; }

	ChangeVolumeSoundMem(volume, data->handle);

	int playType = loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	data->isLooping = loop;

	if (CheckSoundMem(data->handle)) {
		return;
	}

	PlaySoundMem(data->handle, playType, TRUE);
}

void SoundManager::Pause(SRC src)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) { return; }

	StopSoundMem(it->second.handle);
}

void SoundManager::Resume(SRC src)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) { return; }

	SoundData& data = it->second;
	int playType = data.isLooping ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
	PlaySoundMem(data.handle, playType, FALSE);
}

void SoundManager::Stop(SRC src)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) { return; }

	StopSoundMem(it->second.handle);
	SetCurrentPositionSoundMem(0, it->second.handle);
}

void SoundManager::StopAll(void)
{
	for (auto& [src, data] : soundMap_)
	{
		if (data.handle != -1)
		{
			StopSoundMem(data.handle);
		}
	}
}

//=============================================================
// パラメータ変更
//=============================================================

void SoundManager::SetVolume(SRC src, int volume)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) { return; }

	ChangeVolumeSoundMem(volume, it->second.handle);
}

void SoundManager::SetLoop(SRC src, bool loop)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) { return; }

	it->second.isLooping = loop;

	if (CheckSoundMem(it->second.handle))
	{
		int playType = loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK;
		PlaySoundMem(it->second.handle, playType, FALSE);
	}
}

bool SoundManager::IsPlaying(SRC src)
{
	auto it = soundMap_.find(src);
	if (it == soundMap_.end()) { return false; }

	return CheckSoundMem(it->second.handle) == 1;
}
