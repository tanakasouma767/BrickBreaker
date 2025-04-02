#include "GameObject.h"
#include "App.h"

// �l�p�`�̒��_�f�[�^�����֐�
vector<Vertex> CreateRectangleVertices(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color) 
{
	vector<Vertex> vertices =
	{
		{ { pos.x - size.x / 2.0f, pos.y - size.y / 2.0f, 0.0f }, color, { 0.0f, 1.0f } }, // ����
		{ { pos.x + size.x / 2.0f, pos.y - size.y / 2.0f, 0.0f }, color, { 1.0f, 1.0f } }, // �E��
		{ { pos.x - size.x / 2.0f, pos.y + size.y / 2.0f, 0.0f }, color, { 0.0f, 0.0f } }, // ����
		{ { pos.x + size.x / 2.0f, pos.y + size.y / 2.0f, 0.0f }, color, { 1.0f, 0.0f } } // �E��
	};

	return vertices;
}

// �~�̒��_�f�[�^�����֐�
vector<Vertex> CreateCircleVertices(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, int segments) 
{
	vector<Vertex> vertices;

	const float PI = 3.14159265359f;

	// ���S�_
	vertices.push_back({{ pos.x, pos.y, 0.0f }, color});

	// �~����̓_���v�Z
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * PI * i / segments; // �p�x
		float x = size.x / 2 * cosf(theta);
		float y = size.y / 2 * sinf(theta);
		vertices.push_back({{ pos.x + x,  pos.y + y, 0.0f }, color});
	}

	return vertices;
}

// �~�̃C���f�b�N�X�f�[�^�����֐�
vector<uint32_t> CreateCircleIndices(int segments) 
{
	vector<uint32_t> indices;

	for (int i = 1; i <= segments; ++i) {
		indices.push_back(0);       // ���S�_
		indices.push_back(i);       // ���݂̉~����̓_
		indices.push_back(i + 1);   // ���̉~����̓_
	}

	return indices;
}

GameObject::GameObject(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path)
	: position(pos), size(sz), color(col), offset(pos), texPath(path)
{
	// �ϊ��s��
	auto eyePos = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); // ���_�̈ʒu
	auto targetPos = XMVectorZero(); // ���_����������W
	auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // �������\���x�N�g��
	auto fov = XMConvertToRadians(37.5); // ����p
	auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // �A�X�y�N�g��


	// �ϊ��s��p�萔�o�b�t�@�[
	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		constantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		if (!constantBuffer[i]->IsValid())
		{
			printf("�ϊ��s��p�萔�o�b�t�@�̐����Ɏ��s\n");
		}

		// �ϊ��s��̓o�^
		auto ptr = constantBuffer[i]->GetPtr<Transform>();
		ptr->World = XMMatrixIdentity();
		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	// �e�N�X�`���p�f�B�X�N���v�^�q�[�v
	descriptorHeap = new DescriptorHeap();
	auto mainTex = Texture2D::Get(texPath);
	auto handle = descriptorHeap->Register(mainTex);
	materialHandles = handle;
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		delete constantBuffer[i];
	}
	delete descriptorHeap;
}


void GameObject::Draw(RootSignature* rootSignature, PipelineState* pipelineState) 
{
	auto currentIndex = g_Engine->CurrentBackBufferIndex(); // ���݂̃t���[���ԍ����擾����
	auto commandList = g_Engine->CommandList(); // �R�}���h���X�g
	auto materialHeap = descriptorHeap->GetHeap(); // �f�B�X�N���v�^�q�[�v

	auto vbView = vertexBuffer->View();
	auto ibView = indexBuffer->View();

	commandList->SetGraphicsRootSignature(rootSignature->Get());
	commandList->SetPipelineState(pipelineState->Get());
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView);

	commandList->SetDescriptorHeaps(1, &materialHeap); // �g�p����f�B�X�N���v�^�q�[�v���Z�b�g
	commandList->SetGraphicsRootDescriptorTable(1, materialHandles->HandleGPU); // �f�B�X�N���v�^�e�[�u�����Z�b�g

	commandList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}

BoundingBox GameObject::GetBoundingBox() 
{
	DirectX::BoundingBox bbox;
	bbox.Center = { position.x, position.y, 0.0f };
	bbox.Extents = { size.x / 2, size.y / 2, 0.0f };
	return bbox;
}


Bar::Bar(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path) : GameObject(pos, sz, col, path)
{
	// ���_�f�[�^
	vertices = CreateRectangleVertices(pos, sz, col);

	// ���_�o�b�t�@�[
	auto vertSize = sizeof(Vertex) * vertices.size();
	auto vertStride = sizeof(Vertex);
	auto vertPtr = vertices.data();
	auto pVB = new VertexBuffer(vertSize, vertStride, vertPtr);
	if (!pVB->IsValid())
	{
		printf("���_�o�b�t�@�̐����Ɏ��s\n");
	}
	vertexBuffer = pVB;


	// �C���f�b�N�X�f�[�^
	indices = { 0, 1, 2, 1, 2, 3 };

	// �C���f�b�N�X�o�b�t�@�[
	auto indSize = sizeof(uint32_t) * indices.size();
	auto indPtr = indices.data();
	auto pIB = new IndexBuffer(indSize, indPtr);
	if (!pIB->IsValid())
	{
		printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
	}
	indexBuffer = pIB;
}

Bar::~Bar() {
	delete vertexBuffer;
	delete indexBuffer;
}

void Bar::Update(float mouseInputX, float mouseInputY) 
{
	position.x = mouseInputX * 2.8;
	auto ptr = constantBuffer[g_Engine->CurrentBackBufferIndex()]->GetPtr<Transform>();
	ptr->World = XMMatrixTranslation(position.x - offset.x, position.y - offset.y, 0.0f);
}


Block::Block(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path) : GameObject(pos, sz, col, path)
{
	// ���_�f�[�^
	vertices = CreateRectangleVertices(pos, sz, col);

	// ���_�o�b�t�@�[
	auto vertSize = sizeof(Vertex) * vertices.size();
	auto vertStride = sizeof(Vertex);
	auto vertPtr = vertices.data();
	auto pVB = new VertexBuffer(vertSize, vertStride, vertPtr);
	if (!pVB->IsValid())
	{
		printf("���_�o�b�t�@�̐����Ɏ��s\n");
	}
	vertexBuffer = pVB;


	// �C���f�b�N�X�f�[�^
	indices = { 0, 1, 2, 1, 2, 3 };

	// �C���f�b�N�X�o�b�t�@�[
	auto indSize = sizeof(uint32_t) * indices.size();
	auto indPtr = indices.data();
	auto pIB = new IndexBuffer(indSize, indPtr);
	if (!pIB->IsValid())
	{
		printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
	}
	indexBuffer = pIB;
}

Block::~Block() 
{
	delete vertexBuffer;
	delete indexBuffer;
}

Ball::Ball(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path, XMFLOAT2 vel)
	: GameObject(pos, sz, col, path), velocity(vel) 
{

	int segments =  64;
	// ���_�f�[�^
	vertices = CreateCircleVertices(pos, sz, col, segments);

	// ���_�o�b�t�@�[
	auto vertSize = sizeof(Vertex) * vertices.size();
	auto vertStride = sizeof(Vertex);
	auto vertPtr = vertices.data();
	auto pVB = new VertexBuffer(vertSize, vertStride, vertPtr);
	if (!pVB->IsValid())
	{
		printf("���_�o�b�t�@�̐����Ɏ��s\n");
	}
	vertexBuffer = pVB;


	// �C���f�b�N�X�f�[�^
	indices = CreateCircleIndices(segments);

	// �C���f�b�N�X�o�b�t�@�[
	auto indSize = sizeof(uint32_t) * indices.size();
	auto indPtr = indices.data();
	auto pIB = new IndexBuffer(indSize, indPtr);
	if (!pIB->IsValid())
	{
		printf("�C���f�b�N�X�o�b�t�@�̐����Ɏ��s\n");
	}
	indexBuffer = pIB;
} 

Ball::~Ball() 
{
	delete vertexBuffer;
	delete indexBuffer;
}

void Ball::Update(float deltaTime) 
{
	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;

	auto ptr = constantBuffer[g_Engine->CurrentBackBufferIndex()]->GetPtr<Transform>();
	ptr->World = XMMatrixTranslation(position.x - offset.x, position.y - offset.y, 0.0f);
}

void Ball::Reflect(bool reflectX, bool reflectY) 
{
	
	if (reflectX) {
		velocity.x = -velocity.x;
	}

	if (reflectY) {
		velocity.y = -velocity.y;
	}
}

XMFLOAT2 Ball::getVelocity() 
{
	return velocity;
}

void Ball::setVelocity(XMFLOAT2 newVel) 
{
	velocity = newVel;
}