#pragma once
#include "AtlasAnimator.h"
#include "FixedList.h"

class AnimationGroup
{
public:
    AnimationGroup() {}
    void Initialize(AtlasRenderer* renderer, uint8_t numAnimations);
    void Release();

    void Update(float deltaTime);

    AtlasAnimator* AddAnimation(SpriteSheet* frames, float frameRate, bool looped = false, bool rebounds = false, int8_t nextAnimationIndex = -1);
    void StartAnimation(uint8_t index, bool reversed = false);
    AtlasAnimator* GetAnimation(uint8_t index);
    AtlasAnimator* GetCurrentAnimation();

private:
    AtlasRenderer* renderer;
    DynamicFixedList<AtlasAnimator> animators;
    DynamicFixedList<SpriteSheet*> animationSprites;
    DynamicFixedList<uint8_t> nextAnimationIndices; // input index is the current animation, the int at that index is the index of the next animation
    int8_t currentAnimationIndex;
};

