#include "AtlasAnimator.h"

void AtlasAnimator::Initialize(Renderer* atlasRenderer, Animation* animations, int16_t animationCount) {
	this->atlasRenderer = atlasRenderer;
	this->animations = animations;
	this->animationCount = animationCount;
	completed = true;
}

void AtlasAnimator::Update(float deltaTime) {
	if(completed) return;

	const Animation* currentAnimation = animations + animationIndex;
	timer -= currentAnimation->frameRate * deltaTime;
	if(timer > 0.f) return;

	// step frame
	timer += 1.f;

	if(reversed) {
		frameIndex--;
		if(frameIndex < currentAnimation->firstFrameIndex) {
			// determine what to do when reaching the beginning
			if(currentAnimation->looped && currentAnimation->rebounds) {
				// start going forwards
				reversed = false;
				frameIndex = currentAnimation->firstFrameIndex + 1;
			}
			else if(currentAnimation->looped) {
				frameIndex = currentAnimation->lastFrameIndex;
			}
			else {
				completed = true;
				frameIndex = 0;
			}
		}
	} else {
		frameIndex++;
		if(frameIndex > currentAnimation->lastFrameIndex) {
			// determine what to do when reaching the end
			if(currentAnimation->rebounds) {
				// start going backwards
				reversed = true;
				frameIndex = currentAnimation->lastFrameIndex - 1;
			}
			else if(currentAnimation->looped) {
				frameIndex = currentAnimation->firstFrameIndex;
			}
			else {
				completed = true;
				frameIndex = currentAnimation->lastFrameIndex;
			}
		}
	}

	if(currentAnimation->nextAnimationIndex > 0 && currentAnimation->nextAnimationIndex < animationCount)
		Start(currentAnimation->nextAnimationIndex, false);
	else UpdateSprite();
}

void AtlasAnimator::Start(int16_t index, bool reversed) {
	animationIndex = index;
	frameIndex = reversed ? atlasRenderer->atlasData.atlas->SpriteCount() - 1 : 0;
	timer = 1.f;
	this->reversed = reversed;
	completed = false;
	UpdateSprite();
}

void AtlasAnimator::UpdateSprite() {
	atlasRenderer->atlasData.atlas = animations[animationIndex].spriteSheet;
	atlasRenderer->atlasData.row = frameIndex / atlasRenderer->atlasData.atlas->cols;
	atlasRenderer->atlasData.col = frameIndex % atlasRenderer->atlasData.atlas->cols;
}