#pragma once
#include <d3d11.h>
#include <string>
#include <fstream>
#include <sstream>
#include <D3Dcompiler.h>
#include <vector>
#include <map>
#include <wrl/client.h>
#include "constantBuffer.h"
#pragma comment(lib, "d3dcompiler.lib")


class shader1 {
public:
    shader1() : vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr), vsConstantBuffer(nullptr) {}
    ~shader1() {
        if (inputLayout) inputLayout->Release();
        if (vertexShader) vertexShader->Release();
        if (pixelShader) pixelShader->Release();
        if (vsConstantBuffer) vsConstantBuffer->Release();
    }

   
    bool init(ID3D11Device* device, ID3D11DeviceContext* context,
        const std::string& vsFilename, const std::string& psFilename,
        const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT layoutCount,
        UINT vsConstBufferSize = sizeof(VSConstantBuffer))
    {
        this->deviceContext = context;
     
        Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
        if (!compileShader(vsFilename, "VS", "vs_5_0", vsBlob.GetAddressOf())) return false;

        HRESULT hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
        if (FAILED(hr)) return false;

        hr = device->CreateInputLayout(layoutDesc, layoutCount, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
        if (FAILED(hr)) return false;

       
        Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
        if (!compileShader(psFilename, "PS", "ps_5_0", psBlob.GetAddressOf())) return false;

        hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
        if (FAILED(hr)) return false;

     
        D3D11_BUFFER_DESC cbd = {};
        cbd.ByteWidth = vsConstBufferSize;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        hr = device->CreateBuffer(&cbd, nullptr, &vsConstantBuffer);
        if (FAILED(hr)) return false;

        return true;
    }

    void apply() {
        deviceContext->IASetInputLayout(inputLayout);
        deviceContext->VSSetShader(vertexShader, NULL, 0);
        deviceContext->PSSetShader(pixelShader, NULL, 0);

        if (vsConstantBuffer) {
            deviceContext->VSSetConstantBuffers(0, 1, &vsConstantBuffer);
        }
    }

    bool updateVSConstantBuffer(const void* data, size_t dataSize) {
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        HRESULT hr = deviceContext->Map(vsConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr)) return false;
        memcpy(mapped.pData, data, dataSize);
        deviceContext->Unmap(vsConstantBuffer, 0);
        return true;
    }

    void setPSShaderResource(ID3D11ShaderResourceView* srv, UINT slot = 0) {
        deviceContext->PSSetShaderResources(slot, 1, &srv);
    }

    void setPSSampler(ID3D11SamplerState* sampler, UINT slot = 0) {
        deviceContext->PSSetSamplers(slot, 1, &sampler);
    }

private:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    ID3D11Buffer* vsConstantBuffer;
    ID3D11DeviceContext* deviceContext;

    bool compileShader(const std::string& filename, const std::string& entryPoint,
        const std::string& target, ID3DBlob** blob)
    {
        std::ifstream ifs(filename);
        if (!ifs.good()) return false;
        std::stringstream ss;
        ss << ifs.rdbuf();
        std::string code = ss.str();

        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3DCompile(code.c_str(), code.size(), NULL, NULL, NULL,
            entryPoint.c_str(), target.c_str(), 0, 0, blob, errorBlob.GetAddressOf());
        if (FAILED(hr)) {
            if (errorBlob) {
                MessageBoxA(NULL, (char*)errorBlob->GetBufferPointer(), "Shader Compile Error", 0);
            }
            return false;
        }
        return true;
    }
};
#pragma once
