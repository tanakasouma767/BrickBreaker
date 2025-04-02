#include "Scene.h"

using namespace DirectX;

Scene* g_Scene;

bool Scene::Init()
{
	auto currentTime = chrono::high_resolution_clock::now();
	previousTime = currentTime;

	// バーの生成
	XMFLOAT2 rectpos = { 0.0f, -0.5f };
	XMFLOAT2 rectsize = { 0.5f, 0.05f };
	XMFLOAT4 rectcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Bar* bar = new Bar(rectpos, rectsize, rectcolor, L"");
	gameObjects.push_back(bar);

	// ボールの生成
	XMFLOAT2 ballpos = { 0.0f, 0.0f };
	XMFLOAT2 ballsize = { 0.05f, 0.05f };
	XMFLOAT4 ballcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT2 velocity = { 1.0f, 1.0f };
	Ball* ball = new Ball(ballpos, ballsize, ballcolor, L"", velocity);
	gameObjects.push_back(ball);

	// 上壁の生成
	XMFLOAT2 topwallpos = { 0.0f, 1.75f };
	XMFLOAT2 topwallsize = { 8.0f, 0.05f };
	XMFLOAT4 topwallcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Block* topwall = new Block(topwallpos, topwallsize, topwallcolor, L"");
	gameObjects.push_back(topwall);

	// 左壁の生成
	XMFLOAT2 sidewallpos = { -3.1f, 0.0f };
	XMFLOAT2 sidewallsize = { 0.05f, 4.0f };
	XMFLOAT4 sidewallcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Block* sidewall = new Block(sidewallpos, sidewallsize, sidewallcolor, L"");
	gameObjects.push_back(sidewall);

	// 右壁の生成
	XMFLOAT2 sidewallpos2 = { 3.1f, 0.0f };
	Block* sidewall2 = new Block(sidewallpos2, sidewallsize, sidewallcolor, L"");
	gameObjects.push_back(sidewall2);

	// 下壁の生成
	XMFLOAT2 Deadlinepos = { 0.0f, -1.75f };
	Block* Deadline = new Block(Deadlinepos, topwallsize, topwallcolor, L"");
	gameObjects.push_back(Deadline);

	// ブロックの生成
	for (float i = -2.0f; i <= 2.0f; i+=1.0f) {
		for (float j = 0.5; j <= 1.5; j+=0.5) {
			XMFLOAT2 blockPos = { i, j };
			XMFLOAT2 blockSize = { 0.5f, 0.1f };  // 幅1、高さ1
			XMFLOAT4 blockColor = { 1.0f, 1.0f, 1.0f, 1.0f };

			Block* block = new Block(blockPos, blockSize, blockColor, L"");

			gameObjects.push_back(block);
		}
	}

	// 失敗時とクリア時のメッセージ生成
	XMFLOAT2 msgPos = { 0.0, 0.0 };
	XMFLOAT2 msgSize = { 1.0f, 1.0f };  // 幅1、高さ1
	XMFLOAT4 msgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	clearMsg = new Block(msgPos, msgSize, msgColor, L"Assets/clear.png");
	failedMsg = new Block(msgPos, msgSize, msgColor, L"Assets/failed.png");
	

	// ルートシグネチャの生成
	rootSignature = new RootSignature();
	if (!rootSignature->IsValid())
	{
		printf("ルートシグネチャの生成に失敗\n");
		return false;
	}

	// パイプラインステートの生成
	pipelineState = new PipelineState();
	pipelineState->SetInputLayout(Vertex::InputLayout);
	pipelineState->SetRootSignature(rootSignature->Get());
	//pipelineState->SetVS(L"VertexShader.hlsl");
	//pipelineState->SetPS(L"PixelShader.hlsl");
	pipelineState->SetVS(L"./VertexShader.hlsl");
	pipelineState->SetPS(L"./PixelShader.hlsl");
	pipelineState->Create();
	if (!pipelineState->IsValid())
	{
		printf("パイプラインステートの生成に失敗\n");
		return false;
	}

	printf("シーンの初期化に成功\n");
	return true;
}


void Scene::Update(float mouseInputX, float mouseInputY)
{
	// 初期画面
	if (currentState == GameState::InitialScreen) {

		auto currentTime = chrono::high_resolution_clock::now();
		previousTime = currentTime;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			TransitionTo(GameState::Gameplay); // 左クリックでゲーム開始
		}
		return;
	}

	// ゲームプレイメイン画面
	if (currentState == GameState::Gameplay) {

		// バーの移動処理
		Bar* bar = dynamic_cast<Bar*>(gameObjects[0]);
		bar->Update(mouseInputX, mouseInputY);

		// 時刻を更新
		auto currentTime = chrono::high_resolution_clock::now();
		float deltaTime = chrono::duration<float>(currentTime - previousTime).count();
		previousTime = currentTime;

		// ボールの移動処理
		Ball* ball = dynamic_cast<Ball*>(gameObjects[1]);
		ball->Update(deltaTime);

		// 衝突判定(バー)
		if (ball->GetBoundingBox().Intersects(bar->GetBoundingBox())) {
			ball->Reflect(false, true); // Y軸方向に反射
		}

		// 衝突判定(上壁)
		Block* TopWall = dynamic_cast<Block*>(gameObjects[2]);
		if (ball->GetBoundingBox().Intersects(TopWall->GetBoundingBox())) {
			ball->Reflect(false, true); // Y軸方向に反射
		}

		// 衝突判定(横壁)
		for (int i = 3; i <= 4; i++) {
			Block* SideWall = dynamic_cast<Block*>(gameObjects[i]);
			if (ball->GetBoundingBox().Intersects(SideWall->GetBoundingBox())) {
				ball->Reflect(true, false);
			}
		}

		// 衝突判定(下壁)
		Block* Deadline = dynamic_cast<Block*>(gameObjects[5]);
		if (ball->GetBoundingBox().Intersects(Deadline->GetBoundingBox())) {
			TransitionTo(GameState::FailedScreen);
		}

		// 衝突判定(ブロック)
		for (int i = 6; i < gameObjects.size(); ++i) {
			Block* block = dynamic_cast<Block*>(gameObjects[i]);
			if (ball->GetBoundingBox().Intersects(block->GetBoundingBox())) {
				ball->Reflect(false, true);
				delete gameObjects[i];   // ブロックを削除
				gameObjects.erase(gameObjects.begin() + i);
				--i;

				DirectX::XMFLOAT2 vel = ball->getVelocity();
				vel.x *= 1.1;
				vel.y *= 1.1;
				ball->setVelocity(vel);
			}
		}

		// 全ブロックを破壊したらクリア画面に遷移
		if (gameObjects.size() <= 6) {
			TransitionTo(GameState::ClearScreen);
		}
	}

	// クリア画面
	if (currentState == GameState::ClearScreen) {
		if (GetAsyncKeyState(VK_LBUTTON)) {
			printf("Game Restarted\n");
			deleteMenber();
			Init();
			TransitionTo(GameState::InitialScreen); // クリックで初期画面に戻る
		}
	}

	// 失敗画面
	if (currentState == GameState::FailedScreen) {
		if (GetAsyncKeyState(VK_LBUTTON)) {
			printf("Game Restarted\n");
			deleteMenber();
			Init();
			TransitionTo(GameState::InitialScreen); // クリックで初期画面に戻る
		}
	}
}


void Scene::Draw()
{
	// 全ゲームオブジェクトの描画
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Draw(rootSignature, pipelineState);
	}

	// クリア表示
	if (currentState == GameState::ClearScreen) {
		clearMsg->Draw(rootSignature, pipelineState);
	}

	// 失敗表示
	if (currentState == GameState::FailedScreen) {
		failedMsg->Draw(rootSignature, pipelineState);
	}
}

void Scene::TransitionTo(GameState newState)
{
	currentState = newState;

	if (currentState == GameState::Gameplay) {
		printf("Gameplay started\n");
	}
	else if (currentState == GameState::ClearScreen) {
		printf("Clear screen displayed\n");
	}
}

GameState Scene::getState() {
	return currentState;
}

void Scene::deleteMenber()
{
	// 各ゲームオブジェクトのメモリを解放
	for (GameObject* obj : gameObjects)
	{
		delete obj;
	}
	gameObjects.clear(); // ポインタを全て削除後コンテナをクリア

	// クリアメッセージと失敗メッセージを解放
	delete clearMsg;
	clearMsg = nullptr;

	delete failedMsg;
	failedMsg = nullptr;

	// ルートシグネチャとパイプラインステートを解放
	delete rootSignature;
	rootSignature = nullptr;

	delete pipelineState;
	pipelineState = nullptr;
}