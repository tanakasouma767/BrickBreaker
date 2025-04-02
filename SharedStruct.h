#pragma once
#include <d3dx12.h>
#include <DirectXMath.h>
#include "ComPtr.h"
#include <vector>

using namespace std;

struct Vertex
{
    DirectX::XMFLOAT3 Position; // �ʒu���W
    DirectX::XMFLOAT4 Color; // ���_�F
    DirectX::XMFLOAT2 UV; // uv���W

    static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
    static const int InputElementCount = 3;
    static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct alignas(256) Transform
{
    DirectX::XMMATRIX World; // ���[���h�s��
    DirectX::XMMATRIX View; // �r���[�s��
    DirectX::XMMATRIX Proj; // ���e�s��
};

struct Object {
    vector<Vertex> vertices;
    vector<uint32_t> indices;
};