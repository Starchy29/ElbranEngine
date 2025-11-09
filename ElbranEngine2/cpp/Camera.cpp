#include "Camera.h"
#include "Application.h"
#include "GraphicsAPI.h"
#include "Common.h"

float Camera::GetViewHeight() const {
	return viewWidth / app->graphics->GetViewAspectRatio();
}

Vector2 Camera::GetWorldDimensions() const {
	return Vector2(viewWidth, GetViewHeight());
}

AlignedRect Camera::GetViewArea() const {
	ASSERT(transform->rotation == 0.f);
	return AlignedRect(transform->position, GetWorldDimensions());
}