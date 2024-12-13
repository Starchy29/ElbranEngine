#include "PostProcessTexture.h"

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> PostProcessTexture::GetShaderResource() {
    return shaderResourceView;
}

Microsoft::WRL::ComPtr<ID3D11RenderTargetView> PostProcessTexture::GetRenderTarget() {
    return renderTargetView;
}