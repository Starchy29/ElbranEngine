#include "AnimationGroup.h"
#include <assert.h>

AnimationGroup::AnimationGroup(std::vector<SpriteAnimator*> animations) {
	assert(animations.size() > 0 && "animation group contained no animations");
	this->animations = animations;
	currentAnimation = animations[0];
}

void AnimationGroup::Update(float deltaTime) {
	currentAnimation->Update(deltaTime);
}

IBehavior* AnimationGroup::Clone() {
	return new AnimationGroup(animations);
}

void AnimationGroup::SetOwner(GameObject* owner) {
	this->owner = owner;
	for(SpriteAnimator* animation : animations) {
		animation->SetOwner(owner);
	}
}

void AnimationGroup::StartAnimation(int index, bool reversed) {
	currentAnimation = GetAnimation(index);
	currentAnimation->Restart(reversed);
}

SpriteAnimator* AnimationGroup::GetAnimation(int index) {
	assert(index < animations.size() && index >= 0 && "animation index out of range");
	return animations[index];
}
