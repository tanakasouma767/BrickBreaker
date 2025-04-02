#pragma once
#include <cstdint>
#include <d3d12.h>
#include "ComPtr.h"

class IndexBuffer
{
public:
	IndexBuffer(size_t size, const uint32_t* pInitData = nullptr);
	bool IsValid();
	D3D12_INDEX_BUFFER_VIEW View() const;

private:
	bool m_IsValid = false;
	ComPtr<ID3D12Resource> m_pBuffer; // �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW m_View; // �C���f�b�N�X�o�b�t�@�r���[

	IndexBuffer(const IndexBuffer&) = delete;
	void operator = (const IndexBuffer&) = delete;
};