#pragma once
#include "AtlasAnimator.h"

struct AnimationGroup {
    struct Animation {
        AtlasAnimator animator;
        const SpriteSheet* spriteSheet;
        int8_t nextAnimation;
    };

    Renderer* atlasRenderer;
    Animation* animations;
    int8_t currentIndex;
    uint8_t numAnimations;

    AnimationGroup() = default;
    void Initialize(Renderer* atlasRenderer, uint8_t numAnimations);
    void Release();

    void Update(float deltaTime);

    void AddAnimation(const SpriteSheet* frames, float frameRate, bool looped = false, bool rebounds = false, int8_t nextAnimationIndex = -1);
    void StartAnimation(uint8_t index, bool reversed = false);
};