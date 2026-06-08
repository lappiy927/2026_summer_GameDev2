#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "MenuScene.h"

MenuScene::MenuScene(void)
{
}

MenuScene::~MenuScene(void)
{
}

void MenuScene::Init(void)
{
	menuBg_ = LoadGraph("Data/Image/Menu/Menu.png");

	buttonPlay_ = LoadGraph("Data/Image/Menu/Button_Play.png");
	buttonPlaySelect_ = LoadGraph("Data/Image/Menu/Button_Play_Select.png");

	buttonOption_ = LoadGraph("Data/Image/Menu/Button_Option.png");
	buttonOptionSelect_ = LoadGraph("Data/Image/Menu/Button_Option_Select.png");

	buttonExit_ = LoadGraph("Data/Image/Menu/Button_Exit.png");
	buttonExitSelect_ = LoadGraph("Data/Image/Menu/Button_Exit_Select.png");
}

void MenuScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	if (ins.IsTrgDown(KEY_INPUT_UP))
	{
		selectNo_--;

		if (selectNo_ < 0)
		{
			selectNo_ = 2;
		}
	}

	if (ins.IsTrgDown(KEY_INPUT_DOWN))
	{
		selectNo_++;

		if (selectNo_ > 2)
		{
			selectNo_ = 0;
		}
	}

	if (ins.IsTrgDown(KEY_INPUT_RETURN))
	{
		switch (selectNo_)
		{
		case 0:
			// ÉQÅ[ÉÄÇ÷ñﬂÇÈ
			sceMng_.PopScene();
			break;

		case 1:
			// Option
			break;

		case 2:
			// Exit
			sceMng_.ChangeScene(
				SceneManager::SCENE_ID::TITLE);
			break;
		}
	}
}

void MenuScene::Draw(void)
{
	DrawGraph(0, 0, menuBg_, TRUE);

	DrawString(
		0,
		0,
		"MENU",
		0xff0000);

	if (selectNo_ == 0)
	{
		DrawGraph(500, 450, buttonPlaySelect_, TRUE);
	}
	else
	{
		DrawGraph(500, 450, buttonPlay_, TRUE);
	}

	if (selectNo_ == 1)
	{
		DrawGraph(500, 550, buttonOptionSelect_, TRUE);
	}
	else
	{
		DrawGraph(500, 550, buttonOption_, TRUE);
	}

	if (selectNo_ == 2)
	{
		DrawGraph(500, 650, buttonExitSelect_, TRUE);
	}
	else
	{
		DrawGraph(500, 650, buttonExit_, TRUE);
	}
}

void MenuScene::Release(void)
{
}
