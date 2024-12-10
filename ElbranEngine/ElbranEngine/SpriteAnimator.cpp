#include "SpriteAnimator.h"
#include "AtlasRenderer.h"

SpriteAnimator::SpriteAnimator(std::shared_ptr<SpriteAtlas> spriteSheet, int frames, float fps) {
    looped = false;
    oscillates = false;
    reversed = false;
    timer = 0.f;
    SetFramerate(fps);
    completed = false;
    reversed = false;
    currentIndex = 0;

    frameCount = frames;
    this->spriteSheet = spriteSheet;

    assert(frameCount > 1 && "animation contained too few frames");
}

SpriteAnimator::SpriteAnimator(std::vector<std::shared_ptr<Sprite>> spriteList, float fps) {
    looped = false;
    oscillates = false;
    reversed = false;
    timer = 0.f;
    SetFramerate(fps);
    completed = false;
    reversed = false;
    currentIndex = 0;

    this->spriteList = spriteList;
    frameCount = spriteList.size();

    assert(frameCount > 1 && "animation contained too few frames");
}

void SpriteAnimator::SetFramerate(float fps) {
    secsPerFrame = 1.0f / fps;
}

void SpriteAnimator::Restart(bool reversed) {
    completed = false;
    this->reversed = reversed;
    currentIndex = reversed ? frameCount - 1 : 0;

    UpdateSprite();
}

bool SpriteAnimator::IsComplete() {
    return completed;
}

void SpriteAnimator::SetOwner(GameObject* owner) {
    this->owner = owner;
    UpdateSprite();
}

void SpriteAnimator::Update(float deltaTime) {
    timer -= deltaTime;
    if(timer <= 0.f) {
        timer += secsPerFrame;
        StepFrame();
        UpdateSprite();
    }
}

IBehavior* SpriteAnimator::Clone() {
    return new SpriteAnimator(*this);
}

void SpriteAnimator::StepFrame() {
    if(reversed) {
        currentIndex--;

        if(currentIndex >= 0) {
            return;
        }

        if(looped && oscillates) {
            reversed = false;
            currentIndex = 1;
        }
        else if(looped) {
            currentIndex = frameCount - 1;
        }
        else {
            completed = true;
            currentIndex = 0;
        }
    } else {
        currentIndex++;

        if(currentIndex < frameCount) {
            return;
        }

        if(oscillates) {
            // start going backwards
            reversed = true;
            currentIndex = frameCount - 2;
        }
        else if(looped) {
            currentIndex = 0;
        }
        else {
            completed = true;
            currentIndex = frameCount - 1;
        }
    }
}

void SpriteAnimator::UpdateSprite() {
    if(spriteSheet != nullptr) {
        int sheetCols = spriteSheet->NumCols();
        AtlasRenderer* renderer = owner->GetRenderer<AtlasRenderer>();
        renderer->sprite = spriteSheet;
        renderer->row = currentIndex / sheetCols;
        renderer->col = currentIndex % sheetCols;
    } else {
        SpriteRenderer* renderer = owner->GetRenderer<SpriteRenderer>();
        renderer->sprite = spriteList[currentIndex];
    }
}
