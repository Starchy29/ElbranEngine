#pragma once
#include "Buffers.h"
#include "GraphicsAPI.h"

void Mesh::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(vertices);
	graphics->ReleaseData(indices);
}

void ConstantBuffer::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(buffer);
}

void ArrayBuffer::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(buffer);
	graphics->ReleaseData(view);
}

void OutputBuffer::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(gpuBuffer);
	graphics->ReleaseData(cpuBuffer);
	graphics->ReleaseData(view);
}

void Texture2D::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(data);
	graphics->ReleaseData(inputView);
}

void RenderTarget::Release(GraphicsAPI* graphics) {
	Texture2D::Release(graphics);
	graphics->ReleaseData(outputView);
}

void ComputeTexture::Release(GraphicsAPI* graphics) {
	Texture2D::Release(graphics);
	graphics->ReleaseData(outputView);
}

void EditBuffer::Release(GraphicsAPI* graphics) {
	ArrayBuffer::Release(graphics);
	graphics->ReleaseData(computeView);
}
