#include "Scene.h"

using namespace DirectX;

Scene* g_Scene;

bool Scene::Init()
{
	auto currentTime = chrono::high_resolution_clock::now();
	previousTime = currentTime;

	// �o�[�̐���
	XMFLOAT2 rectpos = { 0.0f, -0.5f };
	XMFLOAT2 rectsize = { 0.5f, 0.05f };
	XMFLOAT4 rectcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Bar* bar = new Bar(rectpos, rectsize, rectcolor, L"");
	gameObjects.push_back(bar);

	// �{�[���̐���
	XMFLOAT2 ballpos = { 0.0f, 0.0f };
	XMFLOAT2 ballsize = { 0.05f, 0.05f };
	XMFLOAT4 ballcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT2 velocity = { 1.0f, 1.0f };
	Ball* ball = new Ball(ballpos, ballsize, ballcolor, L"", velocity);
	gameObjects.push_back(ball);

	// ��ǂ̐���
	XMFLOAT2 topwallpos = { 0.0f, 1.75f };
	XMFLOAT2 topwallsize = { 8.0f, 0.05f };
	XMFLOAT4 topwallcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Block* topwall = new Block(topwallpos, topwallsize, topwallcolor, L"");
	gameObjects.push_back(topwall);

	// ���ǂ̐���
	XMFLOAT2 sidewallpos = { -3.1f, 0.0f };
	XMFLOAT2 sidewallsize = { 0.05f, 4.0f };
	XMFLOAT4 sidewallcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Block* sidewall = new Block(sidewallpos, sidewallsize, sidewallcolor, L"");
	gameObjects.push_back(sidewall);

	// �E�ǂ̐���
	XMFLOAT2 sidewallpos2 = { 3.1f, 0.0f };
	Block* sidewall2 = new Block(sidewallpos2, sidewallsize, sidewallcolor, L"");
	gameObjects.push_back(sidewall2);

	// ���ǂ̐���
	XMFLOAT2 Deadlinepos = { 0.0f, -1.75f };
	Block* Deadline = new Block(Deadlinepos, topwallsize, topwallcolor, L"");
	gameObjects.push_back(Deadline);

	// �u���b�N�̐���
	for (float i = -2.0f; i <= 2.0f; i+=1.0f) {
		for (float j = 0.5; j <= 1.5; j+=0.5) {
			XMFLOAT2 blockPos = { i, j };
			XMFLOAT2 blockSize = { 0.5f, 0.1f };  // ��1�A����1
			XMFLOAT4 blockColor = { 1.0f, 1.0f, 1.0f, 1.0f };

			Block* block = new Block(blockPos, blockSize, blockColor, L"");

			gameObjects.push_back(block);
		}
	}

	// ���s���ƃN���A���̃��b�Z�[�W����
	XMFLOAT2 msgPos = { 0.0, 0.0 };
	XMFLOAT2 msgSize = { 1.0f, 1.0f };  // ��1�A����1
	XMFLOAT4 msgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	clearMsg = new Block(msgPos, msgSize, msgColor, L"Assets/clear.png");
	failedMsg = new Block(msgPos, msgSize, msgColor, L"Assets/failed.png");
	

	// ���[�g�V�O�l�`���̐���
	rootSignature = new RootSignature();
	if (!rootSignature->IsValid())
	{
		printf("���[�g�V�O�l�`���̐����Ɏ��s\n");
		return false;
	}

	// �p�C�v���C���X�e�[�g�̐���
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
		printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s\n");
		return false;
	}

	printf("�V�[���̏������ɐ���\n");
	return true;
}


void Scene::Update(float mouseInputX, float mouseInputY)
{
	// �������
	if (currentState == GameState::InitialScreen) {

		auto currentTime = chrono::high_resolution_clock::now();
		previousTime = currentTime;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			TransitionTo(GameState::Gameplay); // ���N���b�N�ŃQ�[���J�n
		}
		return;
	}

	// �Q�[���v���C���C�����
	if (currentState == GameState::Gameplay) {

		// �o�[�̈ړ�����
		Bar* bar = dynamic_cast<Bar*>(gameObjects[0]);
		bar->Update(mouseInputX, mouseInputY);

		// �������X�V
		auto currentTime = chrono::high_resolution_clock::now();
		float deltaTime = chrono::duration<float>(currentTime - previousTime).count();
		previousTime = currentTime;

		// �{�[���̈ړ�����
		Ball* ball = dynamic_cast<Ball*>(gameObjects[1]);
		ball->Update(deltaTime);

		// �Փ˔���(�o�[)
		if (ball->GetBoundingBox().Intersects(bar->GetBoundingBox())) {
			ball->Reflect(false, true); // Y�������ɔ���
		}

		// �Փ˔���(���)
		Block* TopWall = dynamic_cast<Block*>(gameObjects[2]);
		if (ball->GetBoundingBox().Intersects(TopWall->GetBoundingBox())) {
			ball->Reflect(false, true); // Y�������ɔ���
		}

		// �Փ˔���(����)
		for (int i = 3; i <= 4; i++) {
			Block* SideWall = dynamic_cast<Block*>(gameObjects[i]);
			if (ball->GetBoundingBox().Intersects(SideWall->GetBoundingBox())) {
				ball->Reflect(true, false);
			}
		}

		// �Փ˔���(����)
		Block* Deadline = dynamic_cast<Block*>(gameObjects[5]);
		if (ball->GetBoundingBox().Intersects(Deadline->GetBoundingBox())) {
			TransitionTo(GameState::FailedScreen);
		}

		// �Փ˔���(�u���b�N)
		for (int i = 6; i < gameObjects.size(); ++i) {
			Block* block = dynamic_cast<Block*>(gameObjects[i]);
			if (ball->GetBoundingBox().Intersects(block->GetBoundingBox())) {
				ball->Reflect(false, true);
				delete gameObjects[i];   // �u���b�N���폜
				gameObjects.erase(gameObjects.begin() + i);
				--i;

				DirectX::XMFLOAT2 vel = ball->getVelocity();
				vel.x *= 1.1;
				vel.y *= 1.1;
				ball->setVelocity(vel);
			}
		}

		// �S�u���b�N��j�󂵂���N���A��ʂɑJ��
		if (gameObjects.size() <= 6) {
			TransitionTo(GameState::ClearScreen);
		}
	}

	// �N���A���
	if (currentState == GameState::ClearScreen) {
		if (GetAsyncKeyState(VK_LBUTTON)) {
			printf("Game Restarted\n");
			deleteMenber();
			Init();
			TransitionTo(GameState::InitialScreen); // �N���b�N�ŏ�����ʂɖ߂�
		}
	}

	// ���s���
	if (currentState == GameState::FailedScreen) {
		if (GetAsyncKeyState(VK_LBUTTON)) {
			printf("Game Restarted\n");
			deleteMenber();
			Init();
			TransitionTo(GameState::InitialScreen); // �N���b�N�ŏ�����ʂɖ߂�
		}
	}
}


void Scene::Draw()
{
	// �S�Q�[���I�u�W�F�N�g�̕`��
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Draw(rootSignature, pipelineState);
	}

	// �N���A�\��
	if (currentState == GameState::ClearScreen) {
		clearMsg->Draw(rootSignature, pipelineState);
	}

	// ���s�\��
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
	// �e�Q�[���I�u�W�F�N�g�̃����������
	for (GameObject* obj : gameObjects)
	{
		delete obj;
	}
	gameObjects.clear(); // �|�C���^��S�č폜��R���e�i���N���A

	// �N���A���b�Z�[�W�Ǝ��s���b�Z�[�W�����
	delete clearMsg;
	clearMsg = nullptr;

	delete failedMsg;
	failedMsg = nullptr;

	// ���[�g�V�O�l�`���ƃp�C�v���C���X�e�[�g�����
	delete rootSignature;
	rootSignature = nullptr;

	delete pipelineState;
	pipelineState = nullptr;
}