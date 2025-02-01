#include "TextRenderer.h"
#include "Application.h"
using namespace DirectX;

TextRenderer::TextRenderer(std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color) {
	this->text = text;
	this->font = font;
	this->color = color;

	horizontalAlignment = Direction::Center;
	verticalAlignment = Direction::Center;

	vertexShader = nullptr;
	pixelShader = nullptr;
}

void TextRenderer::Draw(Camera* camera, const Transform& transform) {
	// convert from model space to normalized screen coords [-1,1]
	XMFLOAT4X4 screenMatrix;
	GetScreenTransform(&screenMatrix, camera, transform);
	XMMATRIX worldViewProj = XMLoadFloat4x4(&screenMatrix);
	Vector2 center = Vector2::Zero;
	Vector2 right = Vector2(0.5f, 0.0f).Transform(worldViewProj);
	Vector2 top = Vector2(0.0f, 0.5f).Transform(worldViewProj);
	XMFLOAT3 center3D = XMFLOAT3(center.x, center.y, transform.GetGlobalZ());
	XMStoreFloat3(&center3D, XMVector3Transform(XMLoadFloat3(&center3D), worldViewProj));
	center = Vector2(center3D.x, center3D.y);

	// convert from [-1,1] range to [0-pixelWidth]
	XMUINT2 halfScreen = APP->Graphics()->GetViewDimensions();
	halfScreen = XMUINT2(halfScreen.x / 2, halfScreen.y / 2);
	center = Vector2(halfScreen.x + halfScreen.x * center.x, halfScreen.y + halfScreen.y * -center.y);
	right = Vector2(halfScreen.x + halfScreen.x * right.x, halfScreen.y + halfScreen.y * -right.y);
	top = Vector2(halfScreen.x + halfScreen.x * top.x, halfScreen.y + halfScreen.y * -top.y);

	// determine the maximum size that can fit within the box
	Vector2 toRight = right - center;
	Vector2 toTop = top - center;
	float angle = toRight.Angle();

	RECT textSize = font->MeasureDrawBounds(text.c_str(), XMFLOAT2(0, 0));
	float textWidth = (float)(textSize.right - textSize.left);
	float textHeight = (float)(textSize.bottom - textSize.top);

	float horiScale = 2 * toRight.Length() / textWidth;
	float vertScale = 2 * toTop.Length() / textHeight;
	float size = min(horiScale, vertScale);

	// align the text to a side of the box
	float halfWidth;
	if(horizontalAlignment == Direction::Left) {
		halfWidth = textWidth * size / 2.0f;
		center += -toRight + toRight.Normalize() * halfWidth;
	}
	else if(horizontalAlignment == Direction::Right) {
		halfWidth = textWidth * size / 2.0f;
		center += toRight - toRight.Normalize() * halfWidth;
	}

	float halfHeight;
	if(verticalAlignment == Direction::Up) {
		halfHeight = textHeight * size / 2.0f;
		center += toTop - toTop.Normalize() * halfHeight;
	}
	else if(verticalAlignment == Direction::Down) {
		halfHeight = textHeight * size / 2.0f;
		center += -toTop + toTop.Normalize() * halfHeight;
	}

	font->DrawString(APP->Graphics()->GetSpriteBatch(),
		text.c_str(),
		center,
		XMLoadFloat4((XMFLOAT4*)&color),
		angle,
		XMFLOAT2(textWidth / 2.0f + textSize.left, textHeight / 2.0f + textSize.top),
		size,
		DX11::SpriteEffects_None,
		center3D.z
	);
}

IBehavior* TextRenderer::Clone() {
	return new TextRenderer(*this);
}
