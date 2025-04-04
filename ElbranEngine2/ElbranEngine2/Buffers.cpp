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

void Sprite::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(texture);
	graphics->ReleaseData(view);
}

void RenderTarget::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(texture);
	graphics->ReleaseData(inputView);
	graphics->ReleaseData(outputView);
}

void ComputeTexture::Release(GraphicsAPI* graphics) {
	graphics->ReleaseData(texture);
	graphics->ReleaseData(inputView);
	graphics->ReleaseData(outputView);
}
