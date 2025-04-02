#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ComPtr.h"
#include <vector>

using namespace std;

struct Vertex
{
    DirectX::XMFLOAT3 Position; // 位置座標
    DirectX::XMFLOAT4 Color; // 頂点色
    DirectX::XMFLOAT2 UV; // uv座標

    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static const int InputElementCount = 3;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) Transform
{
    DirectX::XMMATRIX World; // ワールド行列
    DirectX::XMMATRIX View; // ビュー行列
    DirectX::XMMATRIX Proj; // 投影行列
};

struct Object {
    vector<Vertex> vertices;
    vector<uint32_t> indices;
};