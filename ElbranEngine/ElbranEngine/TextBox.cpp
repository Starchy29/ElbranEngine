#include "TextBox.h"
#include "Application.h"
using namespace DirectX;

TextBox::TextBox(Scene* scene, float zCoord, std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color)
	: GameObject(scene, RenderMode::Text, zCoord, color)
{
	this->text = text;
	this->font = font;
	maxSize = 1;
}

#include <iostream>
void TextBox::Draw(Camera* camera) {
	XMFLOAT4X4 worldMat = transform.GetWorldMatrix();
	XMFLOAT4X4 viewMat = camera->GetView();
	XMFLOAT4X4 projMat = camera->GetProjection();

	XMMATRIX product = XMLoadFloat4x4(&worldMat);
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&viewMat));
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&projMat));

	// convert from world space to normalized screen coords [-2,2]
	XMFLOAT2 center = transform.GetPosition();
	XMFLOAT2 right = center;
	right.x += transform.GetScale().x / 2.0f;
	XMFLOAT2 top = center;
	top.y += transform.GetScale().y / 2.0f;
	XMFLOAT3 normalizedCenter;
	XMStoreFloat3(&normalizedCenter, XMVector3Transform(XMVectorSet(center.x, center.y, transform.GetGlobalZ(), 0), product));
	XMStoreFloat2(&right, XMVector3Transform(XMVectorSet(right.x, right.y, 0, 0), product));
	XMStoreFloat2(&top, XMVector3Transform(XMVectorSet(top.x, top.y, 0, 0), product));

	std::cout << "x: " + std::to_string(normalizedCenter.x) + ", y: " + std::to_string(normalizedCenter.y) + "\n";

	// convert from [-2,2] range to [0-pixelWidth]
	XMINT2 halfScreen = APP->GetDXCore()->GetViewDimensions();
	halfScreen = XMINT2(halfScreen.x / 2, halfScreen.y / 2);
	XMFLOAT2 screenPos = XMFLOAT2(halfScreen.x + halfScreen.x * normalizedCenter.x / 2.0f, halfScreen.y + halfScreen.y * -normalizedCenter.y / 2.0f);

	// determine the maximum size that can fit within the box
	float size = maxSize;
	RECT textSize = font->MeasureDrawBounds(text.c_str(), XMFLOAT2(0, 0));
	float startArea = textSize.right * textSize.bottom;
	float maxArea = 1;
	if(startArea > maxArea) {
		//size = maxArea / startArea;
	}

	font->DrawString(APP->GetDXCore()->spriteBatch,
		text.c_str(),
		screenPos,
		XMLoadFloat4((XMFLOAT4*)&colorTint),
		0.0f,
		XMFLOAT2(textSize.right / 2.0f, textSize.bottom / 2.0f),
		size,
		DX11::SpriteEffects_None,
		1.0f
	);
}
