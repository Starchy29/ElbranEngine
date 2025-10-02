#pragma once
#include "IRenderer.h"
#include "Font.h"
#include <string>
#include "Common.h"

enum class HorizontalAlignment {
    Left,
    Right,
    Center
};

enum class VerticalAlignment {
    Top,
    Bottom,
    Center
};

class TextRenderer :
    public IRenderer
{
public:
    VerticalAlignment verticalAlignment;
    float padding;
    Color color;

    TextRenderer() {}
    void Initialize(std::string text, const Font* font, HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center, float lineSpacing = 0.0f);
    void Release();

    void Draw() override;

    void SetText(std::string text);
    void SetFont(const Font* font);
    void SetLineSpacing(float spacing);
    void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment);

private:
    const Font* font;
    std::string text;
    float lineSpacing;
    HorizontalAlignment horizontalAlignment;
    Mesh textMesh;
    float blockAspectRatio;

    void GenerateMesh();
};

