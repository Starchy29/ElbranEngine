#include "GameObject.h"

void GameObject::Update() {
	if(!IsActive()) {
		return;
	}

	// for each behavior.. update
}

bool GameObject::IsActive() const {
	return false;
}
