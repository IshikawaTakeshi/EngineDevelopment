#pragma warning(push)
//C4023の警告を見なかったことにする
#pragma warning(disable:4023)
//Include
#include "D3DResourceLeakChecker.h"
#include "WinApp.h"
#include "Logger.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "TextureManager.h"

#include "Transform.h"
#include "MatrixMath.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Model.h"
#include "Camera.h"
#include "CameraManager.h"

#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

#pragma region imgui
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // DEBUG
#pragma endregion


//////////////////////////////////////////////////////////
//Windowsアプリでのエントリーポイント(main関数)
//////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {


	D3DResourceLeakChecker* leakCheck = new D3DResourceLeakChecker;

	//タイトルバーの名前の入力
	WinApp* winApp = new WinApp();
	winApp->Initialize(L"CG2_08_01");

	//DirectX初期化
	DirectXCommon* directXCommon = new DirectXCommon();
	directXCommon->Initialize(winApp);

	//入力初期化
	Input* input = Input::GetInstance();
	input->Initialize(winApp);


#pragma region ImGui初期化
#ifdef _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(directXCommon->GetDevice(),
		directXCommon->GetBufferCount(),
		directXCommon->GetRtvFormat(),
		directXCommon->GetSrvHeap(),
		directXCommon->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
		directXCommon->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart()
	);
#endif // DEBUG
#pragma endregion


	//SpriteCommon
	SpriteCommon* spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(directXCommon);

	//Object3dCommon
	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	object3dCommon->Initialize(directXCommon);

	//ModelManager
	ModelManager::GetInstance()->Initialize(directXCommon);

	//TextureManager
	TextureManager::GetInstance()->Initialize(directXCommon);
	
	//Camera0
	std::shared_ptr<Camera> camera0 = std::make_shared<Camera>();
	camera0->SetTranslate({ 0.0f,0.0f,-20.0f });
	camera0->SetRotate({ 0.0f,0.0f,0.0f });
	CameraManager::GetInstance()->AddCamera(*camera0);

	//Camera1
	std::shared_ptr<Camera> camera1 = std::make_shared<Camera>();
	camera1->SetTranslate({ 5.0f,0.0f,-10.0f });
	camera1->SetRotate({ 0.0f,-0.4f,0.0f });
	CameraManager::GetInstance()->AddCamera(*camera1);

	//デフォルトカメラの設定
	object3dCommon->SetDefaultCamera(CameraManager::GetInstance()->GetActiveCamera());

	//Model読み込み
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("plane.obj");


	//3dObject
	std::shared_ptr<Object3d> object3d = std::make_shared<Object3d>();
	object3d->Initialize(object3dCommon,"axis.obj");
	object3d->SetScale({0.5f,0.5f,0.5f});
	std::shared_ptr<Object3d> object3d1 = std::make_shared<Object3d>();
	object3d1->Initialize(object3dCommon,"plane.obj");
	object3d1->SetScale({ 0.5f,0.5f,0.5f });
	

	/////////////////////////////////////////////////////////////////////////////////////////////
	//										メインループ
	/////////////////////////////////////////////////////////////////////////////////////////////

	//ウィンドウの×ボタンが押されるまでループ

	while (true) {
		//メッセージ処理
		if (winApp->ProcessMessage()) {
			//ウィンドウの×ボタンが押されたらループを抜ける
			break;
		}

		//========================== 更新処理　==========================//

#ifdef _DEBUG
		//ImGui受付開始
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム特有の処理に置き換える
		ImGui::ShowDemoWindow();
#endif // DEBUG

		//入力更新
		input->Update();

		//カメラの更新
		CameraManager::GetInstance()->Update();

		//カメラの切り替え
		if (input->GetInstance()->TriggerKey(DIK_1)) {
			CameraManager::GetInstance()->SetActiveCamera(0);
		} 
		else if (input->GetInstance()->TriggerKey(DIK_2)) {
			CameraManager::GetInstance()->SetActiveCamera(1);
		}

		object3d->Update();
		object3d1->Update();

#ifdef _DEBUG
		//ImGuiの更新
		CameraManager::GetInstance()->UpdateImGui();
		object3d->UpdateImGui(0);
		object3d1->UpdateImGui(1);
#endif // DEBUG

		//========================== 描画処理　==========================//

		
		directXCommon->PreDraw(); //描画前処理
		spriteCommon->PreDraw();  //Spriteの描画前処理

		
		object3dCommon->PreDraw(); //Object3dの描画前処理

		//for (int i = 0; i < 5; i++) {
		//	sprites[i]->DrawCall(); //スプライトの描画
		//}

		
		object3d->Draw(); //3Dオブジェクトの描画
		object3d1->Draw(); //3Dオブジェクトの描画

		//描画後処理
		directXCommon->PostDraw();
	}


	/*==========ImGuiの開放==========*/
#ifdef _DEBUG
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // DEBUG


	//テクスチャマネージャの開放
	TextureManager::GetInstance()->Finalize();
	//ModelManagerの開放
	ModelManager::GetInstance()->Finalize();
	//CameraManagerの開放
	CameraManager::GetInstance()->Finalize();

	//SpriteCommonの開放aa
	spriteCommon->Finalize();

	//Object3dCommonの開放
	object3dCommon->Finalize();

	//入力の開放
	input->Finalize();
	
	//directXCommonの開放
	directXCommon->Finalize();
	delete directXCommon;

	//winAppの開放
	winApp->Finalize();
	delete winApp;
	
	//spritesの開放
	//for (auto& sprite : sprites) {
	//	delete sprite;
	//	sprite = nullptr;
	//}

	delete leakCheck;
	
	return 0;
}



