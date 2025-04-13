#include "Scene.h"
#include "Application.h"
#include "GraphicsAPI.h"

void Scene::Draw() {
	Matrix viewProjection = 
		Matrix::ProjectOrthographic(camera.viewWidth, camera.viewWidth * app->graphics->GetViewAspectRatio(), CAMERA_DEPTH)
		* Matrix::View(camera.position, camera.rotation);

	// set the lights

	// draw opaques front to back

	// draw the background

	// draw translucents back to front
}
