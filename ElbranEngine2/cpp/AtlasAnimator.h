#pragma once
#include "Renderer.h"

struct Animation {
    const SpriteSheet* spriteSheet;
    float frameRate;
    int16_t firstFrameIndex;
    int16_t lastFrameIndex;
    int16_t nextAnimationIndex;
    bool looped;
    bool rebounds;

    Animation() = default;
    inline Animation(const SpriteSheet* spriteSheet, float frameRate, bool looped = false, bool rebounds = false) {
        this->spriteSheet = spriteSheet;
        this->frameRate = frameRate;
        this->looped = looped;
        this->rebounds = rebounds;
        nextAnimationIndex = -1;
        firstFrameIndex = 0;
        lastFrameIndex = spriteSheet->SpriteCount() - 1;
    }
};

struct AtlasAnimator {
public:
    Renderer* atlasRenderer;
    Animation* animations;
    int16_t animationCount;

    int16_t animationIndex;
    int16_t frameIndex;
    bool reversed;
    bool completed;
    float timer;

    AtlasAnimator() = default;
    void Initialize(Renderer* atlasRenderer, Animation* animations, int16_t animationCount);

    void Update(float deltaTime);
    void Start(int16_t index, bool reversed = false);
    void UpdateSprite();
};