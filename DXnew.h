#pragma once
#include "adaptor.h"
#include "DX.h"
#include <DirectXMath.h>
#include <vector>

struct GBufferRenderTarget {
    ID3D11Texture2D* texture;
    ID3D11RenderTargetView* rtv;
    ID3D11ShaderResourceView* srv;
};


class DXcoreDeferred {
public:
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* backbufferRenderTargetView;
    ID3D11Texture2D* backbuffer;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11Texture2D* depthBuffer;

    ID3D11RasterizerState* rasterizerState;


    std::vector<GBufferRenderTarget> gBufferRenderTargets;
    ID3D11Buffer* lightBuffer;
    LightBufferType lightData;

    void init(int width, int height, HWND hwnd, bool window_fullscreen) {
        initDeviceAndSwapChain(width, height, hwnd, window_fullscreen);
        createBackBuffer();
        createDepthBuffer(width, height);
        createRasterizerState();
        createGBuffer(width, height);
        createLightBuffer();
        setViewport(width, height);
    }

    void initDeviceAndSwapChain(int width, int height, HWND hwnd, bool window_fullscreen) {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = window_fullscreen ? FALSE : TRUE;

        Adapter a;
        a.findadapter();
        IDXGIAdapter1* adapter = a.getAdapter();

        D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            adapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            D3D11_CREATE_DEVICE_DEBUG,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &sd,
            &swapChain,
            &device,
            nullptr,
            &deviceContext
        );
        swapChain->SetFullscreenState(window_fullscreen, NULL);
    }

    void createBackBuffer() {
        swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
        device->CreateRenderTargetView(backbuffer, NULL, &backbufferRenderTargetView);
    }

    void createDepthBuffer(int width, int height) {
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width = width;
        depthDesc.Height = height;
        depthDesc.MipLevels = 1;
        depthDesc.ArraySize = 1;
        depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.Usage = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        device->CreateTexture2D(&depthDesc, NULL, &depthBuffer);
        device->CreateDepthStencilView(depthBuffer, NULL, &depthStencilView);
        deviceContext->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);
    }

    void createRasterizerState() {
        D3D11_RASTERIZER_DESC rsDesc = {};
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_BACK;
        device->CreateRasterizerState(&rsDesc, &rasterizerState);
        deviceContext->RSSetState(rasterizerState);
    }

    void createGBuffer(int width, int height) {
        DXGI_FORMAT formats[] = {
            DXGI_FORMAT_R16G16B16A16_FLOAT, // Position
            DXGI_FORMAT_R16G16B16A16_FLOAT, // Normal
            DXGI_FORMAT_R8G8B8A8_UNORM      // Albedo
        };

        for (int i = 0; i < 3; ++i) {
            GBufferRenderTarget rt = {};

            D3D11_TEXTURE2D_DESC texDesc = {};
            texDesc.Width = width;
            texDesc.Height = height;
            texDesc.MipLevels = 1;
            texDesc.ArraySize = 1;
            texDesc.Format = formats[i];
            texDesc.SampleDesc.Count = 1;
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

            device->CreateTexture2D(&texDesc, NULL, &rt.texture);
            device->CreateRenderTargetView(rt.texture, NULL, &rt.rtv);
            device->CreateShaderResourceView(rt.texture, NULL, &rt.srv);

            gBufferRenderTargets.push_back(rt);
        }
    }

    void createLightBuffer() {
        lightData.light.ambientColor = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
        lightData.light.diffuseColor = DirectX::XMFLOAT4(2.0f, 2.0f, 2.0f, 1.0f);
        lightData.light.lightDirection = DirectX::XMFLOAT3(-0.5f, -1.0f, 0.0f);

        D3D11_BUFFER_DESC cbDesc = {};
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.ByteWidth = sizeof(LightBufferType);
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        device->CreateBuffer(&cbDesc, NULL, &lightBuffer);
    }

    void setViewport(int width, int height) {
        D3D11_VIEWPORT viewport = {};
        viewport.Width = (float)width;
        viewport.Height = (float)height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        deviceContext->RSSetViewports(1, &viewport);
    }

    void renderGeometryPass() {
        ID3D11RenderTargetView* rtvs[] = {
            gBufferRenderTargets[0].rtv, 
            gBufferRenderTargets[1].rtv, 
            gBufferRenderTargets[2].rtv  
        };

        deviceContext->OMSetRenderTargets(3, rtvs, depthStencilView);
        clearGBuffer();
    }

    void renderLightPass() {
        deviceContext->OMSetRenderTargets(1, &backbufferRenderTargetView, nullptr);

       
        ID3D11ShaderResourceView* srvs[] = {
            gBufferRenderTargets[0].srv,
            gBufferRenderTargets[1].srv,
            gBufferRenderTargets[2].srv
        };
        deviceContext->PSSetShaderResources(0, 3, srvs);
    }

    void clearGBuffer() {
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        for (auto& rt : gBufferRenderTargets) {
            deviceContext->ClearRenderTargetView(rt.rtv, clearColor);
        }
        deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    void present() {
        swapChain->Present(0, 0);
    }
};

