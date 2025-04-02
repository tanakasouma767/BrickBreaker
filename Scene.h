#pragma once
#include "Engine.h"
#include "App.h"
#include <d3dx12.h>
#include "SharedStruct.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include <vector>
#include "ComPtr.h"
#include "GameObject.h"
#include <chrono>
#include "Texture2D.h"

using namespace std;

enum class GameState
{
	InitialScreen,
	Gameplay,
	ClearScreen,
	FailedScreen
};

class Scene
{
private:
	GameState currentState; // シーンの状態
	RootSignature* rootSignature; // ルートシグネチャ
	PipelineState* pipelineState; // パイプラインステート
	vector<GameObject*> gameObjects; // ゲームオブジェクト
	chrono::time_point<chrono::high_resolution_clock> previousTime; // 前回取得した時刻

	Block* clearMsg;	// クリアメッセージ
	Block* failedMsg;	// 失敗時メッセージ

public:
	Scene() : currentState(GameState::InitialScreen) {}
	bool Init(); // 初期化
	void Update(float mouseInputX, float mouseInputY); // 更新処理
	void Draw(); // 描画処理
	void TransitionTo(GameState newState); // シーンの状態遷移
	GameState getState(); // 状態の取得
	void deleteMenber();
};

extern Scene* g_Scene;