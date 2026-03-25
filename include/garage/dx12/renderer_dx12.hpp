#ifndef GARAGE_RENDERERDX11_HPP
#define GARAGE_RENDERERDX11_HPP

#include "renderer.hpp"

namespace Garage
{
	class RendererDX12 : public Renderer
	{
    private:
        IDXGIFactory4* dxgiFactory;

        IDXGISwapChain3* dxgiSwapChain;

        IDXGIDevice* dxgiDevice;

        bool  isMSAA4xEnable;

        UINT m_nMsaa4xQualityLevels = 0;

        ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
        UINT                  m_nRtvDescriptorIncrementSize;

        ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
        UINT                  m_nDsvDescriptorIncrementSize;

        ID3D12CommandAllocator    *m_pd3dCommandAllocator;
        ID3D12GraphicsCommandList *m_pd3dCommandList;

        UINT64 m_nFenceValue;
        HANDLE m_hFenceEvent;

        D3D12_VIEWPORT m_d3dViewport;
        D3D12_RECT m_d3dScissorRect;
	};
}

#endif