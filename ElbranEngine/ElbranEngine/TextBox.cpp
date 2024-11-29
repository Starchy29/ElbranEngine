#include "TextBox.h"
#include "Application.h"
using namespace DirectX;

TextBox::TextBox(float zCoord, std::string text, std::shared_ptr<DirectX::DX11::SpriteFont> font, Color color)
	: GameObject(RenderMode::Text, zCoord, color)
{
	this->text = text;
	this->font = font;
	maxSize = 1;
}

void TextBox::Draw(Camera* camera) {
	XMFLOAT4X4 worldMat = transform.GetWorldMatrix();
	XMFLOAT4X4 viewMat = camera->GetView();
	XMFLOAT4X4 projMat = camera->GetProjection();
	XMMATRIX product = XMLoadFloat4x4(&worldMat);
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&viewMat));
	product = XMMatrixMultiply(product, XMLoadFloat4x4(&projMat));

	// convert from model space to normalized screen coords [-1,1]
	Vector2 center = Vector2::Zero;
	Vector2 right = Vector2(0.5f, 0.0f);
	Vector2 top = Vector2(0.0f, 0.5f);
	XMFLOAT3 center3D;
	center3D.x = center.x;
	center3D.y = center.y;
	center3D.z = transform.GetGlobalZ();
	XMStoreFloat3(&center3D, XMVector3Transform(XMLoadFloat3(&center3D), product));
	XMStoreFloat2(&right, XMVector3Transform(XMLoadFloat2(&right), product));
	XMStoreFloat2(&top, XMVector3Transform(XMLoadFloat2(&top), product));
	center = Vector2(center3D.x, center3D.y);

	// convert from [-1,1] range to [0-pixelWidth]
	XMINT2 halfScreen = APP->Graphics()->GetViewDimensions();
	halfScreen = XMINT2(halfScreen.x / 2, halfScreen.y / 2);
	center = Vector2(halfScreen.x + halfScreen.x * center.x, halfScreen.y + halfScreen.y * -center.y);
	right = Vector2(halfScreen.x + halfScreen.x * right.x, halfScreen.y + halfScreen.y * -right.y);
	top = Vector2(halfScreen.x + halfScreen.x * top.x, halfScreen.y + halfScreen.y * -top.y);

	// determine the maximum size that can fit within the box
	Vector2 toRight = right - center;
	Vector2 toTop = top - center;
	float angle = toRight.Angle();

	RECT textSize = font->MeasureDrawBounds(text.c_str(), XMFLOAT2(0, 0));
	float horiScale = 2 * toRight.Length() / textSize.right;
	float vertScale = 2 * toTop.Length() / textSize.bottom;
	float size = min(horiScale, vertScale);
	if(size > maxSize) {
		size = maxSize;
	}

	// align the text to a side of the box
	float halfWidth;
	if(horizontalAlignment == Direction::Left) {
		halfWidth = textSize.right * size / 2.0f;
		center += -toRight + toRight.Normalize() * halfWidth;
	}
	else if(horizontalAlignment == Direction::Right) {
		halfWidth = textSize.right * size / 2.0f;
		center += toRight - toRight.Normalize() * halfWidth;
	}

	float halfHeight;
	if(verticalAlignment == Direction::Up) {
		halfHeight = textSize.bottom * size / 2.0f;
		center += toTop - toTop.Normalize() * halfHeight;
	}
	else if(verticalAlignment == Direction::Down) {
		halfHeight = textSize.bottom * size / 2.0f;
		center += -toTop + toTop.Normalize() * halfHeight;
	}

	font->DrawString(APP->Graphics()->spriteBatch,
		text.c_str(),
		center,
		XMLoadFloat4((XMFLOAT4*)&colorTint),
		angle,
		XMFLOAT2(textSize.right / 2.0f, textSize.bottom / 2.0f),
		size,
		DX11::SpriteEffects_None,
		center3D.z
	);
}
