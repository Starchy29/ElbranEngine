#include "Shaders.h"

#ifdef WINDOWS
#include <unknwn.h>
#define RELEASE(x) if(x) ((IUnknown*)x)->Release();
#endif

void Shader::Release() {
	for(int i = 0; i < numConstBuffers; i++) {
		constantBuffers[i].Release();
	}
}

void VertexShader::Release() {
	Shader::Release();
	RELEASE(shader);
}

void GeometryShader::Release() {
	Shader::Release();
	RELEASE(shader);
}

void PixelShader::Release() {
	Shader::Release();
	RELEASE(shader);
}

void ComputeShader::Release() {
	Shader::Release();
	RELEASE(shader);
}
