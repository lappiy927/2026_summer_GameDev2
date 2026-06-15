#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include"../Manager/SoundManager.h"
#include "SceneBase.h"

SceneBase::SceneBase(void)
	:
	resMng_(ResourceManager::GetInstance()),
	sceMng_(SceneManager::GetInstance()),
	sndMng_(SoundManager::GetInstance())
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}
