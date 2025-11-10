#include "AtlasAnimator.h"

/*#define LAST_FRAME renderer->atlas->spriteCount - 1

void AtlasAnimator::Initialize(AtlasRenderer* renderer, float frameRate) {
	this->renderer = renderer;
	this->frameRate = frameRate;
	looped = false;
	rebounds = false;

	Restart();
}

void AtlasAnimator::Update(float deltaTime) {
	if(completed) return;

	timer -= frameRate * deltaTime;
	if(timer > 0.f) return;

	// step frame
	timer += 1.f;

	if(reversed) {
		currentIndex--;
		if(currentIndex < 0) {
			// determine what to do when reaching the beginning
			if(looped && rebounds) {
				// start going forwards
				reversed = false;
				currentIndex = 1;
			}
			else if(looped) {
				currentIndex = LAST_FRAME;
			}
			else {
				completed = true;
				currentIndex = 0;
			}
		}
	} else {
		currentIndex++;
		if(currentIndex >= renderer->atlas->spriteCount) {
			// determine what to do when reaching the end
			if(rebounds) {
				// start going backwards
				reversed = true;
				currentIndex = LAST_FRAME - 1;
			}
			else if(looped) {
				currentIndex = 0;
			}
			else {
				completed = true;
				currentIndex = LAST_FRAME;
			}
		}
	}

	UpdateSprite();
}

void AtlasAnimator::Restart(bool reversed) {
	timer = 1.f;
	currentIndex = reversed ? LAST_FRAME : 0;
	this->reversed = reversed;
	completed = false;

	UpdateSprite();
}

bool AtlasAnimator::IsComplete() const {
	return completed;
}

void AtlasAnimator::UpdateSprite() {
	renderer->row = currentIndex / renderer->atlas->cols;
	renderer->col = currentIndex % renderer->atlas->cols;
}*/