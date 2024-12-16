#pragma once
#include "adaptor.h"
#include <DirectXMath.h>
#include "GEbase.h"
#include <fstream>
#include <iostream>

struct Light {
    DirectX::XMFLOAT4 ambientColor;
    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT3 lightDirection;
    float padding; 
};

struct Wave
{
    float time;
    float waveAmplitude;
    float waveFrequency;
    float waveSpeed;
    DirectX::XMFLOAT2 waveDirection;
    float padding[2];
};
struct TimeBuffer
{
    float time;
};

struct LightBufferType {
    Light light;
};
struct WaveBufferType {
    Wave wave;
};

class DXcore {
public:
    GamesEngineeringBase::Timer tim;
    ID3D11Device* device;
    ID3D11DeviceContext* devicecontext;
    IDXGISwapChain* swapchain;
    ID3D11RenderTargetView* backbufferRenderTargetView;
    ID3D11Texture2D* backbuffer;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11Texture2D* depthbuffer;

    ID3D11RasterizerState* rasterizerState;

   
    ID3D11Buffer* lightBuffer;
    ID3D11Buffer* waveConstantBuffer;
   
    LightBufferType lightData;
    WaveBufferType waveData;
    TimeBuffer timeData;
    
    float dt = tim.dt();

    void init(int width, int height, HWND hwnd, bool window_fullscreen, DXcore& dxcore) {
        DXGI_SWAP_CHAIN_DESC sd;
        memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));
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
        sd.Windowed = window_fullscreen ? false : true;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;

        Adapter a;
        a.findadapter();
        IDXGIAdapter1* adapter = a.getAdapter();

        D3D11CreateDeviceAndSwapChain(adapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            NULL,
            D3D11_CREATE_DEVICE_DEBUG,
            &fl,
            1,
            D3D11_SDK_VERSION,
            &sd,
            &swapchain,
            &device,
            NULL,
            &devicecontext);
        swapchain->SetFullscreenState(window_fullscreen, NULL);

       
        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
        device->CreateRenderTargetView(backbuffer, NULL, &backbufferRenderTargetView);

        
        D3D11_TEXTURE2D_DESC dsvDesc;
        dsvDesc.Width = width;
        dsvDesc.Height = height;
        dsvDesc.MipLevels = 1;
        dsvDesc.ArraySize = 1;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.SampleDesc.Count = 1;
        dsvDesc.SampleDesc.Quality = 0;
        dsvDesc.Usage = D3D11_USAGE_DEFAULT;
        dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        dsvDesc.CPUAccessFlags = 0;
        dsvDesc.MiscFlags = 0;
        device->CreateTexture2D(&dsvDesc, NULL, &depthbuffer);
        device->CreateDepthStencilView(depthbuffer, NULL, &depthStencilView);
        devicecontext->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);

        // 设置视口
        D3D11_VIEWPORT viewport;
        viewport.Width = (float)width;
        viewport.Height = (float)height;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        devicecontext->RSSetViewports(1, &viewport);

    
        D3D11_RASTERIZER_DESC rsdesc;
        ZeroMemory(&rsdesc, sizeof(D3D11_RASTERIZER_DESC));
        rsdesc.FillMode = D3D11_FILL_SOLID;
        rsdesc.CullMode = D3D11_CULL_NONE;
        device->CreateRasterizerState(&rsdesc, &rasterizerState);
        devicecontext->RSSetState(rasterizerState);
        enableAlphaBlending(dxcore);

        createLightBuffer();
        updateLightBuffer();
        createwaveBuffer();
        

        
        
    }

    void createLightBuffer() {
        
        lightData.light.ambientColor = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
        lightData.light.diffuseColor = DirectX::XMFLOAT4(2.0f, 2.0f, 2.0f, 2.0f);
        lightData.light.lightDirection = DirectX::XMFLOAT3(-0.5f, -1.0f, 0.0f);
        lightData.light.padding = 0.0f;

        D3D11_BUFFER_DESC cbDesc;
        ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.ByteWidth = sizeof(LightBufferType);
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = 0;
        cbDesc.MiscFlags = 0;

        HRESULT hr = device->CreateBuffer(&cbDesc, NULL, &lightBuffer);
        if (FAILED(hr)) {
           
        }
    }
    void createwaveBuffer() {
        waveData.wave.time = 0;   
        waveData.wave.waveAmplitude = 0.5f;
        waveData.wave.waveFrequency = 2.0f;
        waveData.wave.waveSpeed = 5.0f;
        waveData.wave.waveDirection = DirectX::XMFLOAT2(1.0f, 0.5f);
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(WaveBufferType);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &waveConstantBuffer);
        if (FAILED(hr)) {
            std::cerr << "Failed to create constant buffer: HRESULT = " << hr << std::endl;
        }
    }
    void LogTimeToFile(float timeValue) {
        std::ofstream logFile("log.txt", std::ios::app);  
        if (logFile.is_open()) {
            logFile << "Current Time: " << timeValue << std::endl;
            logFile.close();
        }
        else {
            std::cerr << "无法打开日志文件" << std::endl;
        }
    }
    void updatewaveBuffer(float time) {
        WaveBufferType waveData = { time,0.5f, 2.0f, 5.0f, DirectX::XMFLOAT2(1.0f, 0.5f) };
        LogTimeToFile(time);
       
        devicecontext->UpdateSubresource(waveConstantBuffer, 0, NULL, &waveData, 0, 0);

        
        
        devicecontext->VSSetConstantBuffers(2, 1, &waveConstantBuffer);
    }

   
    void updateLightBuffer() {
        
        devicecontext->UpdateSubresource(lightBuffer, 0, NULL, &lightData, 0, 0);

       
        devicecontext->PSSetConstantBuffers(1, 1, &lightBuffer);
    }

    void present() {
        swapchain->Present(0, 0);
    }

    void clear() {
        float ClearColour[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
        devicecontext->ClearRenderTargetView(backbufferRenderTargetView, ClearColour);
        devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.0f,
            0);
    }

    void enableAlphaBlending(DXcore& core) {
        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* blendState;
        device->CreateBlendState(&blendDesc, &blendState);

        float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        devicecontext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
    }
};
