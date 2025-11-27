#pragma once
#include "AtlasAnimator.h"

class AnimationGroup
{
public:
    Renderer* atlasRenderer;
    AtlasAnimator* animators;
    const SpriteSheet** spriteSheets;
    uint8_t* animationSequence; // input index is the current animation, the int at that index is the index of the next animation
    int8_t currentAnimationIndex;
    uint8_t numAnimations;

    AnimationGroup() = default;
    void Initialize(Renderer* atlasRenderer, uint8_t numAnimations);
    void Release();

    void Update(float deltaTime);

    AtlasAnimator* AddAnimation(const SpriteSheet* frames, float frameRate, bool looped = false, bool rebounds = false, int8_t nextAnimationIndex = -1);
    void StartAnimation(uint8_t index, bool reversed = false);
};