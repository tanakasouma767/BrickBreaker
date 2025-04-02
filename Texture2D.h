#pragma once
#include "ComPtr.h"
#include <string>
#include <d3dx12.h>

class DescriptorHeap;
class DescriptorHandle;

class Texture2D
{
public:
	static Texture2D* Get(std::string path); // string�Ŏ󂯎�����p�X����e�N�X�`����ǂݍ���
	static Texture2D* Get(std::wstring path); // wstring�Ŏ󂯎�����p�X����e�N�X�`����ǂݍ���
	static Texture2D* GetWhite(); // ���̒P�F�e�N�X�`���𐶐�����
	bool IsValid(); // ����ɓǂݍ��܂�Ă��邩�ǂ�����Ԃ�

	ID3D12Resource* Resource(); // ���\�[�X��Ԃ�
	D3D12_SHADER_RESOURCE_VIEW_DESC ViewDesc(); // �V�F�[�_�[���\�[�X�r���[�̐ݒ��Ԃ�

private:
	bool m_IsValid; // ����ɓǂݍ��܂�Ă��邩
	Texture2D(std::string path);
	Texture2D(std::wstring path);
	Texture2D(ID3D12Resource* buffer);
	ComPtr<ID3D12Resource> m_pResource; // ���\�[�X
	bool Load(std::string& path);
	bool Load(std::wstring& path);

	static ID3D12Resource* GetDefaultResource(size_t width, size_t height);

	Texture2D(const Texture2D&) = delete;
	void operator = (const Texture2D&) = delete;
};
