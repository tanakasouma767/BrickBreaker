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

// ゲームオブジェクトベースクラス
class GameObject 
{
protected:
    XMFLOAT2 position; // 2D空間での位置
    XMFLOAT2 size;     // サイズ（幅・高さ）
    XMFLOAT4 color;    // RGBA色情報
    XMFLOAT2 offset;   // 初期位置
    vector<Vertex> vertices;    // 頂点
    vector<uint32_t> indices;   // インデックス
    VertexBuffer* vertexBuffer; // 頂点バッファ
    IndexBuffer* indexBuffer;   // インデックスバッファ
    ConstantBuffer* constantBuffer[Engine::FRAME_BUFFER_COUNT]; // 定数バッファ
    DescriptorHeap* descriptorHeap; // ディスクリプタヒープ
    DescriptorHandle* materialHandles; // テクスチャ用のハンドル
    const wchar_t* texPath;     // テクスチャのファイルパス

public:
    GameObject(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path);
    virtual ~GameObject();

    virtual void Update(float deltaTime){}
    virtual void Update(float mouseInputX, float mouseInputY){}
    void Draw(RootSignature* rootSignature, PipelineState* pipelineState);
    BoundingBox GetBoundingBox();
};

// バークラス
class Bar : public GameObject 
{
public:
    Bar(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path);
    ~Bar();

    void Update(float mouseInputX, float mouseInputY) override;
};

// ブロッククラス
class Block : public GameObject 
{
public:
    Block(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path);
    ~Block();
};

// ボールクラス
class Ball : public GameObject 
{
private:
    XMFLOAT2 velocity; // ボールの速度

public:
    Ball(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path, XMFLOAT2 vel);
    ~Ball();

    void Update(float deltaTime) override;
    void Reflect(bool reflectY, bool reflectX);
    XMFLOAT2 getVelocity();
    void setVelocity(XMFLOAT2 newVel);
};