#include "AnimationGroup.h"
#include <cassert>

void AnimationGroup::Initialize(AtlasRenderer* renderer, uint8_t numAnimations) {
	this->renderer = renderer;
	currentAnimationIndex = -1;

	animators.Allocate(numAnimations);
	animationSprites.Allocate(numAnimations);
	nextAnimationIndices.Allocate(numAnimations);
}

void AnimationGroup::Release() {
	animators.Release();
	animationSprites.Release();
	nextAnimationIndices.Release();
}

void AnimationGroup::Update(float deltaTime) {
	if(currentAnimationIndex < 0) return;
	animators[currentAnimationIndex].Update(deltaTime);
	if(nextAnimationIndices[currentAnimationIndex] >= 0 && animators[currentAnimationIndex].IsComplete()) {
		StartAnimation(nextAnimationIndices[currentAnimationIndex]);
	}
}

AtlasAnimator* AnimationGroup::AddAnimation(SpriteSheet* frames, float frameRate, bool looped, bool rebounds, int8_t nextAnimationIndex) {
	animationSprites.Add(frames);
	nextAnimationIndices.Add(nextAnimationIndex);

	AtlasAnimator* added = animators.ReserveNext();
	added->Initialize(renderer, frameRate);
	added->looped = looped;
	added->rebounds = rebounds;
	return added;
}

AtlasAnimator* AnimationGroup::GetAnimation(uint8_t index) {
	assert(index < animators.Size() && "animation index was out of range");
	return &animators[index];
}

AtlasAnimator* AnimationGroup::GetCurrentAnimation() {
	if(currentAnimationIndex < 0) return nullptr;
	return &animators[currentAnimationIndex];
}

void AnimationGroup::StartAnimation(uint8_t index, bool reversed) {
	currentAnimationIndex = index;
	renderer->atlas = animationSprites[index];
	animators[index].Restart(reversed);
}
