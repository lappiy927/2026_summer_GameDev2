#include "VertexRenderer.h"

VertexRenderer::VertexRenderer(VertexMaterial& material) :material_(material)
{
}

VertexRenderer::~VertexRenderer(void)
{
}

void VertexRenderer::Draw(int modelId)
{
    MV1SetUseOrigShader(TRUE);
    SetUseVertexShader(material_.GetVS());
    SetUsePixelShader(material_.GetPS());

    // 定数バッファ更新
    int constBuf = material_.GetConstBuf();
    FLOAT4* ptr = (FLOAT4*)GetBufferShaderConstantBuffer(constBuf);
    const auto& constBufs = material_.GetConstBufs();

    for (int i = 0; i < (int)constBufs.size(); i++)
    {
        if (i != 0) ptr++;
        ptr->x = constBufs[i].x;
        ptr->y = constBufs[i].y;
        ptr->z = constBufs[i].z;
        ptr->w = constBufs[i].w;
    }
    UpdateShaderConstantBuffer(constBuf);

    // 頂点シェーダーにセット(スロット8?)
    SetShaderConstantBuffer(constBuf, DX_SHADERTYPE_VERTEX, CONSTANT_BUF_SLOT_BEGIN_VS);

    // ピクセルシェーダーにセット(スロット4?)
    SetShaderConstantBuffer(constBuf, DX_SHADERTYPE_PIXEL, 4);

    MV1DrawModel(modelId);

    MV1SetUseOrigShader(FALSE);
    SetUseVertexShader(-1);
    SetUsePixelShader(-1);
}

