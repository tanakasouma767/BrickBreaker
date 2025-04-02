#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include <vector>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Engine.h"
#include "SharedStruct.h"
#include <DirectXCollision.h>
#include "DescriptorHeap.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "Texture2D.h"

using namespace std;
using namespace DirectX;

// �Q�[���I�u�W�F�N�g�x�[�X�N���X
class GameObject 
{
protected:
    XMFLOAT2 position; // 2D��Ԃł̈ʒu
    XMFLOAT2 size;     // �T�C�Y�i���E�����j
    XMFLOAT4 color;    // RGBA�F���
    XMFLOAT2 offset;   // �����ʒu
    vector<Vertex> vertices;    // ���_
    vector<uint32_t> indices;   // �C���f�b�N�X
    VertexBuffer* vertexBuffer; // ���_�o�b�t�@
    IndexBuffer* indexBuffer;   // �C���f�b�N�X�o�b�t�@
    ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // �萔�o�b�t�@
    DescriptorHeap* descriptorHeap; // �f�B�X�N���v�^�q�[�v
    DescriptorHandle* materialHandles; // �e�N�X�`���p�̃n���h��
    const wchar_t* texPath;     // �e�N�X�`���̃t�@�C���p�X

public:
    GameObject(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path);
    virtual ~GameObject();

    virtual void Update(float deltaTime){}
    virtual void Update(float mouseInputX, float mouseInputY){}
    void Draw(RootSignature* rootSignature, PipelineState* pipelineState);
    BoundingBox GetBoundingBox();
};

// �o�[�N���X
class Bar : public GameObject 
{
public:
    Bar(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path);
    ~Bar();

    void Update(float mouseInputX, float mouseInputY) override;
};

// �u���b�N�N���X
class Block : public GameObject 
{
public:
    Block(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path);
    ~Block();
};

// �{�[���N���X
class Ball : public GameObject 
{
private:
    XMFLOAT2 velocity; // �{�[���̑��x

public:
    Ball(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path, XMFLOAT2 vel);
    ~Ball();

    void Update(float deltaTime) override;
    void Reflect(bool reflectY, bool reflectX);
    XMFLOAT2 getVelocity();
    void setVelocity(XMFLOAT2 newVel);
};