#pragma once
#include "IBehavior.h"
#include "SpriteAnimator.h"

class AnimationGroup :
    public IBehavior
{
public:
    AnimationGroup(std::vector<SpriteAnimator*> animations);

    virtual void Update(float deltaTime) override;
    virtual IBehavior* Clone() override;
    virtual void SetOwner(GameObject* owner) override;

    void StartAnimation(int index, bool reversed = false);
    SpriteAnimator* GetAnimation(int index);

private:
    std::vector<SpriteAnimator*> animations;
    SpriteAnimator* currentAnimation;
};

