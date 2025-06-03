#pragma once
#include "IRenderer.h"
#include "Font.h"
#include <string>

class TextRenderer :
    public IRenderer
{
public:
    TextRenderer(std::string text, const Font* font, float size);

private:

};

