#pragma once
#include "AtlasRenderer.h"

class AtlasAnimator
{
public:
    float frameRate;
    bool looped;
    bool rebounds;

    AtlasAnimator() = default;
    void Initialize(AtlasRenderer* renderer, float frameRate);

    void Update(float deltaTime);
    
    void Restart(bool reversed = false);
    bool IsComplete() const;

private:
    AtlasRenderer* renderer;

    float timer;
    int16_t currentIndex;
    bool reversed;
    bool completed;

    inline void UpdateSprite();
};

