#include "Sprite.h"
#include "NewGame.h"

Sprite::Sprite(std::wstring fileName) {
	GameInstance->LoadTexture(fileName, resourceView.GetAddressOf());

	ID3D11Resource* texture;
	resourceView.Get()->GetResource(&texture);
	
	D3D11_TEXTURE2D_DESC description;
	((ID3D11Texture2D*)texture)->GetDesc(&description);

	pixelWidth = description.Width;
	pixelHeight = description.Height;
	aspectRatio = (float)pixelWidth / pixelHeight;

	texture->Release();
}

int Sprite::GetPixelWidth() {
	return pixelWidth;
}

int Sprite::GetPixelHeight() {
	return pixelHeight;
}

float Sprite::GetAspectRatio() {
	return aspectRatio;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sprite::GetResourceView() {
	return resourceView;
}