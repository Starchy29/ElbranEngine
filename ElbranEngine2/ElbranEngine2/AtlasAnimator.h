#pragma once
#include "IBehavior.h"
#include "AtlasRenderer.h"

class AtlasAnimator :
    public IBehavior
{
public:
    float frameRate;
    bool looped;
    bool rebounds;

    AtlasAnimator() {}
    AtlasAnimator(AtlasRenderer* renderer, float frameRate);

    void Update(float deltaTime) override;
    
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

