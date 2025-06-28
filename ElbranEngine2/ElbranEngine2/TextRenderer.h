#pragma once
#include "IRenderer.h"
#include "Font.h"
#include <string>
#include "Common.h"

class TextRenderer :
    public IRenderer
{
public:
    struct {
        Direction horizontalAlignment;
        Direction verticalAlignment;
        float padding;
    } positioning;

    Color color;
    unsigned int hiddenChars; // number of omitted characters from the end of the text

    TextRenderer(std::string text, const Font* font);

    void Draw() override;

    void SetText(std::string text);
    void SetFont(const Font* font);

private:
    const Font* font;
    std::string text;
    Mesh textMesh;

    void GenerateMesh();
};

