#include "PostProcessTexture.h"

void PostProcessTexture::Resize(Microsoft::WRL::ComPtr<ID3D11Device> device, float width, float height) {
	shaderResourceView.Reset();
	renderTargetView.Reset();
	if(texture) {
		texture->Release();
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	device->CreateTexture2D(&textureDesc, 0, texture.GetAddressOf());
	device->CreateRenderTargetView(texture.Get(), 0, renderTargetView.GetAddressOf());
	device->CreateShaderResourceView(texture.Get(), 0, shaderResourceView.GetAddressOf());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PostProcessTexture::GetShaderResource() {
    return shaderResourceView;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> PostProcessTexture::GetRenderTarget() {
    return renderTargetView;
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> PostProcessTexture::GetTexture() {
	return texture;
}
