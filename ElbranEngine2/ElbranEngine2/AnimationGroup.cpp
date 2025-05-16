#include "AnimationGroup.h"
#include <cassert>

AnimationGroup::AnimationGroup(AtlasRenderer* renderer, unsigned int numAnimations) {
	this->renderer = renderer;
	animators = FixedList<AtlasAnimator>(numAnimations);
	animationSprites = FixedList<SpriteSheet*>(numAnimations);
	nextAnimationIndices = FixedList<int>(numAnimations);

	currentAnimationIndex = -1;
}

AnimationGroup::~AnimationGroup() {
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

AtlasAnimator* AnimationGroup::AddAnimation(SpriteSheet* frames, float frameRate, bool looped, bool rebounds, int nextAnimationIndex) {
	animationSprites.Add(frames);
	nextAnimationIndices.Add(nextAnimationIndex);
	animators.Add(AtlasAnimator(renderer, frameRate));
	AtlasAnimator* added = &animators[animators.GetSize() - 1];
	added->looped = looped;
	added->rebounds = rebounds;
	return added;
}

AtlasAnimator* AnimationGroup::GetAnimation(unsigned int index) {
	assert(index < animators.GetSize() && "animation index was out of range");
	return &animators[index];
}

AtlasAnimator* AnimationGroup::GetCurrentAnimation() {
	if(currentAnimationIndex < 0) return nullptr;
	return &animators[currentAnimationIndex];
}

void AnimationGroup::StartAnimation(int index, bool reversed) {
	currentAnimationIndex = index;
	renderer->atlas = animationSprites[index];
	animators[index].Restart(reversed);
}
