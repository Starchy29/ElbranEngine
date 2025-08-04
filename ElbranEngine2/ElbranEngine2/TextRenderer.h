#pragma once
#include "IRenderer.h"
#include "Font.h"
#include <string>
#include "Common.h"

class TextRenderer :
    public IRenderer
{
public:
    Direction horizontalAlignment;
    Direction verticalAlignment;
    float padding;
    Color color;

    TextRenderer(std::string text, const Font* font);
    ~TextRenderer();

    void Draw() override;

    void SetText(std::string text);
    void SetFont(const Font* font);

private:
    const Font* font;
    std::string text;
    Mesh textMesh;
    float blockAspectRatio;

    void GenerateMesh();
};

