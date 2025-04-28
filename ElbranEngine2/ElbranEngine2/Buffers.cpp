#pragma once
#include "Buffers.h"

#ifdef WINDOWS
#include <unknwn.h>
#define RELEASE(x) if(x) ((IUnknown*)x)->Release();
#endif


void Mesh::Release() {
	RELEASE(vertices);
	RELEASE(indices);
}

void ConstantBuffer::Release() {
	RELEASE(data)
}

void ArrayBuffer::Release() {
	RELEASE(buffer);
	RELEASE(view);
}

void OutputBuffer::Release() {
	RELEASE(gpuBuffer);
	RELEASE(cpuBuffer);
	RELEASE(view);
}

void Texture2D::Release() {
	RELEASE(data);
	RELEASE(inputView);
}

void RenderTarget::Release() {
	Texture2D::Release();
	RELEASE(outputView);
}

void ComputeTexture::Release() {
	Texture2D::Release();
	RELEASE(outputView);
}

void EditBuffer::Release() {
	ArrayBuffer::Release();
	RELEASE(computeView);
}

void Sampler::Release() {
	RELEASE(state);
}

void SpriteSheet::Release() {
	texture.Release();
}
