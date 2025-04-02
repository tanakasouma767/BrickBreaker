#include "PipelineState.h"
#include "Engine.h"
#include <d3dx12.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

PipelineState::PipelineState()
{
	// �p�C�v���C���X�e�[�g�̐ݒ�
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // ���X�^���C�U�[�̓f�t�H���g
	desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O�͂Ȃ�
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // �u�����h�X�e�[�g���f�t�H���g
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // �[�x�X�e���V���̓f�t�H���g���g��
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // �O�p�`��`��
	desc.NumRenderTargets = 1; // �`��Ώۂ�1
	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1; // �T���v���[��1
	desc.SampleDesc.Quality = 0;
}

bool PipelineState::IsValid()
{
	return m_IsValid;
}

void PipelineState::SetInputLayout(D3D12_INPUT_LAYOUT_DESC layout)
{
	desc.InputLayout = layout;
}

void PipelineState::SetRootSignature(ID3D12RootSignature* rootSignature)
{
	desc.pRootSignature = rootSignature;
}

void PipelineState::SetVS(std::wstring filePath)
{
	// ���_�V�F�[�_�[�ǂݍ���
	auto hr = D3DCompileFromFile(
		filePath.c_str(), // �V�F�[�_�[�� ���_�V�F�[�_�[
		nullptr, // define�Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�̓f�t�H���g
		"VSmain", // VS�֐�
		"vs_5_0", // �ΏۃV�F�[�_�[
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�A�œK���Ȃ�
		0,
		m_pVsBlob.GetAddressOf(), // �Ώۃ|�C���^�[
		errorBlob.GetAddressOf() // �G���[���b�Z�[�W�p�|�C���^�[
	);

	if (FAILED(hr))
	{
		printf("���_�V�F�[�_�[�̓ǂݍ��݂Ɏ��s");
		return;
	}

	desc.VS = CD3DX12_SHADER_BYTECODE(m_pVsBlob.Get());
}

void PipelineState::SetPS(std::wstring filePath)
{
	// �s�N�Z���V�F�[�_�[�ǂݍ���
	auto hr = D3DCompileFromFile(
		filePath.c_str(), // �V�F�[�_�[�� �s�N�Z���V�F�[�_�[
		nullptr, // define�Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�̓f�t�H���g
		"PSmain", // PS�֐�
		"ps_5_0", // �ΏۃV�F�[�_�[
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p�A�œK���Ȃ�
		0,
		m_pPSBlob.GetAddressOf(), // �Ώۃ|�C���^�[
		errorBlob.GetAddressOf() // �G���[���b�Z�[�W�p�|�C���^�[
	);

	if (FAILED(hr))
	{
		printf("�s�N�Z���V�F�[�_�[�̓ǂݍ��݂Ɏ��s");
		return;
	}

	desc.PS = CD3DX12_SHADER_BYTECODE(m_pPSBlob.Get());
}

void PipelineState::Create()
{
	// �p�C�v���C���X�e�[�g�𐶐�
	auto hr = g_Engine->Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		printf("�p�C�v���C���X�e�[�g�̐����Ɏ��s");
		return;
	}

	m_IsValid = true;
}

ID3D12PipelineState* PipelineState::Get()
{
	return m_pPipelineState.Get();
}
