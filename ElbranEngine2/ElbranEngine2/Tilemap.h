#pragma once
#include "IRenderer.h"
#include "Buffers.h"
#include "Common.h"

enum class TileType : uint8_t {
    None = 0b00000000,
    Floor = 0b00000001,
    Wall = 0b00000010,
    Pit = 0b00000100,
    Any = 0b00000111
};
TileType operator&(TileType a, TileType b);
TileType operator|(TileType a, TileType b);


class Tilemap :
    public IRenderer
{
public:
    const SpriteSheet* skin;

    void Initialize(uint16_t width, uint16_t height);
    void Release();

    void Draw() override;

    void SetTile(Int2 index, TileType type);

    inline bool IsInBounds(Int2 index) const;
    TileType GetTile(Int2 index) const;
    TileType GetTile(Vector2 worldPosition) const;
    Int2 GetIndex(Vector2 worldPosition) const;
    AlignedRect GetTileArea(Int2 index) const;
    uint16_t GetWidth() const;
    uint16_t GetHeight() const;

private:
    enum class TileImage : uint32_t {
        None,
        Wall,
        Pit
    };

    uint16_t width;
    uint16_t height;
    TileType* tiles;
    TileImage* images;

    ArrayBuffer tileIndices;

    void UpdateAppearance();
};

