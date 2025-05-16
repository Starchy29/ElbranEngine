#pragma once
#include "IBehavior.h"
#include "AtlasAnimator.h"
#include "FixedList.h"

class AnimationGroup :
    public IBehavior
{
public:
    AnimationGroup(AtlasRenderer* renderer, unsigned int numAnimations);
    ~AnimationGroup();

    void Update(float deltaTime) override;

    AtlasAnimator* AddAnimation(SpriteSheet* frames, float frameRate, bool looped = false, bool rebounds = false, int nextAnimationIndex = -1);
    void StartAnimation(int index, bool reversed = false);
    AtlasAnimator* GetAnimation(unsigned int index);
    AtlasAnimator* GetCurrentAnimation();

private:
    AtlasRenderer* renderer;
    FixedList<AtlasAnimator> animators;
    FixedList<SpriteSheet*> animationSprites;
    FixedList<int> nextAnimationIndices; // input index is the current animation, the int at that index is the index of the next animation
    int currentAnimationIndex;
};

