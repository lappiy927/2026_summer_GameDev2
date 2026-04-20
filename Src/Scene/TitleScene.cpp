#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
//#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
//#include "../Object/Common/AnimationController.h"
#include "../Application.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	imgTitle_(-1),
	imgPushSpace_(-1),
	animationController_(nullptr),
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE).handleId_;
	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::PUSH_SPACE).handleId_;

	// 定点カメラ
	//sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	

	//アニメーションコントローラー
	/*animationController_ =
		new AnimationController(charactor_.modelId);
	animationController_->Add(0, 20.0f,
		(Application::PATH_MODEL + "Player/Run.mv1").c_str());
	animationController_->Play(0, true);*/

}

void TitleScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	//アニメーションコントローラー更新
	//animationController_->Update();

}

void TitleScene::Draw(void)
{
	DrawString(0, 0, "TitleScene", 000000);

	//タイトル画像の描画
	DrawRotaGraphF(Application::SCREEN_SIZE_X / 2,
		IMG_TITLE_POS_Y, 1.0f, 0.0f, imgTitle_, TRUE);
	
	//PushSpace画像の描画
	DrawRotaGraphF(Application::SCREEN_SIZE_X / 2,
		IMG_PUSH_POS_Y, 1.0f, 0.0f, imgPushSpace_, TRUE);

}

void TitleScene::Release(void)
{

}