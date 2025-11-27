#include "AnimationGroup.h"

void AnimationGroup::Initialize(Renderer* atlasRenderer, uint8_t numAnimations) {
	this->atlasRenderer = atlasRenderer;
	this->numAnimations = numAnimations;
	currentAnimationIndex = -1;

	animators = (AtlasAnimator*)calloc(numAnimations, sizeof(AtlasAnimator) + sizeof(SpriteSheet*) + sizeof(uint8_t));
}

void AnimationGroup::Release() {
	free(animators);
}

void AnimationGroup::Update(float deltaTime) {
	if(currentAnimationIndex < 0) return;
	animators[currentAnimationIndex].Update(deltaTime);
	if(animationSequence[currentAnimationIndex] >= 0 && animators[currentAnimationIndex].IsComplete()) {
		StartAnimation(animationSequence[currentAnimationIndex]);
	}
}

AtlasAnimator* AnimationGroup::AddAnimation(const SpriteSheet* frames, float frameRate, bool looped, bool rebounds, int8_t nextAnimationIndex) {
	spriteSheets[numAnimations] = frames;
	animationSequence[numAnimations] = nextAnimationIndex;

	animators[numAnimations].Initialize(atlasRenderer, frameRate);
	animators[numAnimations].looped = looped;
	animators[numAnimations].rebounds = rebounds;

	numAnimations++;
	return &animators[numAnimations];
}

void AnimationGroup::StartAnimation(uint8_t index, bool reversed) {
	currentAnimationIndex = index;
	atlasRenderer->atlasData.atlas = spriteSheets[index];
	animators[index].Restart(reversed);
}