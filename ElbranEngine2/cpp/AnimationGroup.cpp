#include "AnimationGroup.h"

void AnimationGroup::Initialize(Renderer* atlasRenderer, uint8_t numAnimations) {
	this->atlasRenderer = atlasRenderer;
	this->numAnimations = numAnimations;
	currentIndex = -1;
	animations = (Animation*)calloc(numAnimations, sizeof(Animation));
}

void AnimationGroup::Release() {
	free(animations);
}

void AnimationGroup::Update(float deltaTime) {
	if(currentIndex < 0) return;
	animations[currentIndex].animator.Update(deltaTime);
	if(animations[currentIndex].nextAnimation >= 0 && animations[currentIndex].animator.IsComplete()) {
		StartAnimation(animations[currentIndex].nextAnimation);
	}
}

void AnimationGroup::AddAnimation(const SpriteSheet* frames, float frameRate, bool looped, bool rebounds, int8_t nextAnimationIndex) {
	animations[numAnimations].spriteSheet = frames;
	animations[numAnimations].nextAnimation = nextAnimationIndex;

	animations[numAnimations].animator.Initialize(atlasRenderer, frameRate);
	animations[numAnimations].animator.looped = looped;
	animations[numAnimations].animator.rebounds = rebounds;

	numAnimations++;
}

void AnimationGroup::StartAnimation(uint8_t index, bool reversed) {
	currentIndex = index;
	atlasRenderer->atlasData.atlas = animations[index].spriteSheet;
	animations[index].animator.Restart(reversed);
}