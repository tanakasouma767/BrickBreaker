#pragma once
#include "ComPtr.h"

struct ID3D12RootSignature;

class RootSignature
{
public:
	RootSignature(); // �R���X�g���N�^�Ń��[�g�V�O�l�`���𐶐�
	bool IsValid(); // ���[�g�V�O�l�`���̐����ɐ����������ǂ�����Ԃ�
	ID3D12RootSignature* Get(); // ���[�g�V�O�l�`����Ԃ�

private:
	bool m_IsValid = false; // ���[�g�V�O�l�`���̐����ɐ����������ǂ���
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr; // ���[�g�V�O�l�`��
};


