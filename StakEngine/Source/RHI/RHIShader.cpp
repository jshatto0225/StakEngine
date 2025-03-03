#include "RHIShader.h"

FRHIShader::FRHIShader(TRef<IRHIShader> Shader) {
    Impl = Shader;
}

void FRHIShader::Shutdown() {
    Impl->Shutdown();
}

ERHIShaderType FRHIShader::GetType() {
    return Impl->GetType();
}