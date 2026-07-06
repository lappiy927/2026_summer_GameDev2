#include "../Application.h"
#include "VertexMaterial.h"

VertexMaterial::VertexMaterial(
    std::string vsFileName, std::string psFileName, int constBufFloat4Size)
{
    shaderVS_ = LoadVertexShader(
        (Application::PATH_SHADER + vsFileName).c_str());
    shaderPS_ = LoadPixelShader(
        (Application::PATH_SHADER + psFileName).c_str());

    constBufFloat4Size_ = constBufFloat4Size;
    constBuf_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * constBufFloat4Size);
}

VertexMaterial::~VertexMaterial(void)
{
    DeleteShader(shaderVS_);
    DeleteShader(shaderPS_);
    DeleteShaderConstantBuffer(constBuf_);
}

void VertexMaterial::AddConstBuf(const FLOAT4& constBuf)
{
    if (constBufFloat4Size_ > (int)constBufs_.size())
    {
        constBufs_.emplace_back(constBuf);
    }
}

void VertexMaterial::SetConstBuf(int idx, const FLOAT4& constBuf)
{
    if (idx >= (int)constBufs_.size()) return;
    constBufs_[idx] = constBuf;
}

int VertexMaterial::GetVS(void) const { return shaderVS_; }
int VertexMaterial::GetPS(void) const { return shaderPS_; }
int VertexMaterial::GetConstBuf(void) const { return constBuf_; }
const std::vector<FLOAT4>& VertexMaterial::GetConstBufs(void) const { return constBufs_; }

