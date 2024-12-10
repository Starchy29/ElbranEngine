#pragma once
#include "IBehavior.h"
#include "SpriteAtlas.h"
#include <memory>
#include <vector>

class SpriteAnimator :
    public IBehavior
{
public:
    bool looped;
    bool oscillates;
    bool reversed;

    SpriteAnimator(std::shared_ptr<SpriteAtlas> spriteSheet, int frames, float fps);
    SpriteAnimator(std::vector<std::shared_ptr<Sprite>> spriteList, float fps);

    void SetFramerate(float fps);
    void Restart(bool reversed = false);
    bool IsComplete();

    virtual void SetOwner(GameObject* owner) override;
    virtual void Update(float deltaTime) override;
    virtual IBehavior* Clone() override;

private:
    std::shared_ptr<SpriteAtlas> spriteSheet;
    std::vector<std::shared_ptr<Sprite>> spriteList;

    float secsPerFrame;
    float timer;
    int frameCount;
    int currentIndex;
    bool completed;

    void StepFrame();
    void UpdateSprite();
};

