#include "Shaders.h"

#ifdef WINDOWS
#include <unknwn.h>
#define RELEASE(x) if(x) ((IUnknown*)x)->Release();
#endif

void VertexShader::Release() {
	RELEASE(shader);
	constants.Release();
}

void GeometryShader::Release() {
	RELEASE(shader);
	constants.Release();
}

void PixelShader::Release() {
	RELEASE(shader);
	constants.Release();
}

void ComputeShader::Release() {
	RELEASE(shader);
	constants.Release();
}
