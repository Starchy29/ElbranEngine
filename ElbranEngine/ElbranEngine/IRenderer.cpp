#include "IRenderer.h"
using namespace DirectX;

void IRenderer::GetScreenTransform(DirectX::XMFLOAT4X4* output, Camera* camera, const Transform& transform) {
	// ignoring the view matrix causes it to be rendered relative to the camera
	const XMFLOAT4X4* cameraMatrix = screenSpace ? camera->GetProjection() : camera->GetViewProjection();
	XMStoreFloat4x4(output, XMMatrixMultiply(XMLoadFloat4x4(transform.GetWorldMatrix()), XMLoadFloat4x4(cameraMatrix)));
}
