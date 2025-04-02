#include "VertexBuffer.h"
#include "Engine.h"
#include <d3dx12.h>

VertexBuffer::VertexBuffer(size_t size, size_t stride, const void* pInitData)
{

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); 	// �q�[�v�v���p�e�B
	auto desc = CD3DX12_RESOURCE_DESC::Buffer(size); 	// ���\�[�X�̐ݒ�

	// ���\�[�X�𐶐�
	auto hr = g_Engine->Device()->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pBuffer.GetAddressOf()));

	if (FAILED(hr))
	{
		printf("���_�o�b�t�@���\�[�X�̐����Ɏ��s");
		return;
	}

	// ���_�o�b�t�@�r���[�̐ݒ�
	m_View.BufferLocation = m_pBuffer->GetGPUVirtualAddress();
	m_View.SizeInBytes = static_cast<UINT>(size);
	m_View.StrideInBytes = static_cast<UINT>(stride);

	// �}�b�s���O����
	if (pInitData != nullptr)
	{
		void* ptr = nullptr;
		hr = m_pBuffer->Map(0, nullptr, &ptr);
		if (FAILED(hr))
		{
			printf("���_�o�b�t�@�}�b�s���O�Ɏ��s");
			return;
		}

		// ���_�f�[�^���}�b�s���O��ɐݒ�
		memcpy(ptr, pInitData, size);

		// �}�b�s���O����
		m_pBuffer->Unmap(0, nullptr);
	}

	m_IsValid = true;
}

void VertexBuffer::UpdateBuffer(size_t size, size_t stride, const void* data)
{
	void* mappedPtr = nullptr;
	D3D12_RANGE readRange = { 0, 0 };
	HRESULT hr = m_pBuffer->Map(0, &readRange, &mappedPtr);
	if (FAILED(hr))
	{
		printf("���_�o�b�t�@�̍X�V�Ɏ��s\n");
		return;
	}

	memcpy(mappedPtr, data, size);
	m_pBuffer->Unmap(0, nullptr);
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::View() const
{
	return m_View;
}

bool VertexBuffer::IsValid()
{
	return m_IsValid;
}

