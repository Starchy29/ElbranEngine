#include "Tilemap.h"
#include "ShaderConstants.h"
#include "Application.h"
#include "GraphicsAPI.h"

#define TILE_HALF_WIDTH 0.5f

void Tilemap::Initialize(uint16_t width, uint16_t height) {
	this->width = width;
	this->height = height;

	// assumes this has been added to a scene already
	transform->scale = Vector2(width, height);
	transform->position = transform->scale * 0.5f;

	tiles = new TileType[width * height];
	images = new TileImage[width * height] {};
	tileIndices = app.graphics->CreateArrayBuffer(ShaderDataType::UInt, width * height);

	for(uint32_t i = 0; i < width * height; i++) {
		tiles[i] = TileType::Floor;
	}

	SetTile({0, 0}, TileType::Wall);
	SetTile({2, 1}, TileType::Pit);
	SetTile({3, 1}, TileType::Pit);
	SetTile({3, 2}, TileType::Pit);
	SetTile({2, 2}, TileType::Wall);
	SetTile({5, 0}, TileType::Wall);
	SetTile({5, 2}, TileType::Wall);
	UpdateAppearance();
}

void Tilemap::Release() {
	delete[] tiles;
	delete[] images;
	app.graphics->ReleaseArrayBuffer(&tileIndices);
}

void Tilemap::Draw() {
	GraphicsAPI* graphics = app.graphics;

	CameraVSConstants vsInput;
	vsInput.worldTransform = worldMatrix->Transpose();
	vsInput.uvOffset = Vector2::Zero;
	vsInput.uvScale = Vector2(1.0f, 1.0f);
	graphics->SetVertexShader(&app.assets.cameraVS, &vsInput, sizeof(CameraVSConstants));

	//TilemapPSConstants psInput;
	//psInput.atlasCols = skin->cols;
	//psInput.tileCols = width;
	//psInput.spriteDims = Vector2(1.0f / skin->cols, 1.0f / skin->rows);
	//graphics->SetTexture(ShaderStage::Pixel, &skin->texture, 0);
	//graphics->SetArray(ShaderStage::Pixel, &tileIndices, 1);
	//graphics->SetPixelShader(&app.assets.tilemapPS, &psInput, sizeof(TilemapPSConstants));

	graphics->DrawMesh(&app.assets.unitSquare);
}

void Tilemap::SetTile(Int2 index, TileType type) {
	tiles[width * index.y + index.x] = type;

	switch(type) {
	case TileType::Floor:
		images[width * index.y + index.x] = TileImage::None;
		break;
	case TileType::Wall:
		images[width * index.y + index.x] = TileImage::Wall;
		break;
	case TileType::Pit:
		images[width * index.y + index.x] = TileImage::Pit;
		break;
	}
}

bool Tilemap::IsInBounds(Int2 index) const {
	return index.x >= 0 && index.y >= 0 && index.x < width && index.y < height;
}

TileType Tilemap::GetTile(Int2 index) const {
	if(!IsInBounds(index)) return TileType::Wall;
	return tiles[index.x + width * index.y];
}

TileType Tilemap::GetTile(Vector2 worldPosition) const {
	return GetTile(GetIndex(worldPosition));
}

Int2 Tilemap::GetIndex(Vector2 worldPosition) const {
	return Int2(worldPosition.x, worldPosition.y);
}

AlignedRect Tilemap::GetTileArea(Int2 index) const {
	return AlignedRect(index.x, index.x + 1.0f, index.y + 1.0f, index.y);
}

uint16_t Tilemap::GetWidth() const {
	return width;
}

uint16_t Tilemap::GetHeight() const {
	return height;
}

void Tilemap::UpdateAppearance() {
	app.graphics->WriteBuffer(images, sizeof(uint32_t) * width * height, tileIndices.buffer);
}

TileType operator&(TileType a, TileType b) {
	return (TileType)((uint8_t)a & (uint8_t)b);
}

TileType operator|(TileType a, TileType b) {
	return (TileType)((uint8_t)a | (uint8_t)b);
}
