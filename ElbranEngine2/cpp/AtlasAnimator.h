#pragma once
#include "Renderer.h"

class AtlasAnimator
{
public:
    float frameRate;
    bool looped;
    bool rebounds;

    AtlasAnimator() = default;
    void Initialize(Renderer* atlasRenderer, float frameRate);

    void Update(float deltaTime);
    
    void Restart(bool reversed = false);
    bool IsComplete() const;

private:
    Renderer* atlasRenderer;

    float timer;
    int16_t currentIndex;
    bool reversed;
    bool completed;

    inline void UpdateSprite();
};