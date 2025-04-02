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
	GameState currentState; // �V�[���̏��
	RootSignature* rootSignature; // ���[�g�V�O�l�`��
	PipelineState* pipelineState; // �p�C�v���C���X�e�[�g
	vector<GameObject*> gameObjects; // �Q�[���I�u�W�F�N�g
	chrono::time_point<chrono::high_resolution_clock> previousTime; // �O��擾��������

	Block* clearMsg;	// �N���A���b�Z�[�W
	Block* failedMsg;	// ���s�����b�Z�[�W

public:
	Scene() : currentState(GameState::InitialScreen) {}
	bool Init(); // ������
	void Update(float mouseInputX, float mouseInputY); // �X�V����
	void Draw(); // �`�揈��
	void TransitionTo(GameState newState); // �V�[���̏�ԑJ��
	GameState getState(); // ��Ԃ̎擾
	void deleteMenber();
};

extern Scene* g_Scene;