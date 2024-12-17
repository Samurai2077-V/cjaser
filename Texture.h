#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include"Math.h"
#include"DX.h"
#include"Shaders.h"
#include"GEMLoader.h"

class Sampler {
public:
	ID3D11SamplerState* state;
	void init(DXcore& dxcore) {
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		dxcore.device->CreateSamplerState(&samplerDesc, &state);
	}
	void bind(DXcore& core) {
		core.devicecontext->PSSetSamplers(0, 1, &state);
	}
};

class Texture {
public:
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
	Sampler sampler;
	void init(DXcore* core, int width, int height, int channels, unsigned char* data, DXGI_FORMAT format) {
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA initData;
		memset(&initData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		initData.pSysMem = data;
		initData.SysMemPitch = width * channels;
		core->device->CreateTexture2D(&texDesc, &initData, &texture);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		core->device->CreateShaderResourceView(texture, &srvDesc, &srv);
	}

	void load(std::string filename, DXcore* dxcore, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM) {
		int width = 0, height = 0, channels = 0;
		unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		init(dxcore, width, height, 4, texels, format);  
		stbi_image_free(texels);
		sampler.init(*dxcore);
		sampler.bind(*dxcore);
	}
	void free() {
		srv->Release();
		texture->Release();
	}
};

class TextureManager
{
public:
	std::map<std::string, Texture*> textures;
	void load(DXcore* core, std::string filename, bool isNormalMap = false)
	{
		if (textures.find(filename) != textures.end()) {
			return;
		}
		Texture* texture = new Texture();
		DXGI_FORMAT format = isNormalMap ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		texture->load(filename, core, format);  
		textures.insert({ filename, texture });
	}
	void load1(DXcore* core, std::string filename, bool isNormalMap = false)
	{
		if (textures.find(filename) != textures.end()) {
			return;
		}

		Texture* texture = new Texture();
		DXGI_FORMAT format = isNormalMap ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		texture->load(filename, core, format);  
		textures.insert({ filename, texture });
	}
	ID3D11ShaderResourceView* find(std::string name)
	{
		return textures[name]->srv;
	}
	ID3D11ShaderResourceView* find1(std::string name)
	{
		std::string normalMapName = name + "_Normal";  

		auto it = textures.find(normalMapName);
		if (it != textures.end() && it->second != nullptr) {
			return it->second->srv;
		}
		return nullptr;  
	}
	void unload(std::string name)
	{
		textures[name]->free();
		textures.erase(name);
	}
	~TextureManager()
	{
		for (auto it = textures.cbegin(); it != textures.cend(); )
		{
			it->second->free();
			textures.erase(it++);
		}
	}
};
