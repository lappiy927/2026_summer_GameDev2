#include "../Common/VertexToPixelHeader.hlsli"

#define PS_INPUT VertexToPixelLit
#include "../Common/Pixel/PixelShader3DHeader.hlsli"

cbuffer cbD3D11_CONST_BUFFER_PS_RIMLIGHT : register(b4)
{
    float4 rimColor; // rgb: リムカラー, a: 強度(intensity)
    float4 rimParams; // x: 指数(power), y: 有効/無効, z,w: 未使用
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 col = diffuseMapTexture.Sample(diffuseMapSampler, input.uv);
    col *= input.diffuse;

    clip(col.a - 0.5f);

    if (rimParams.y > 0.5f)
    {
        float3 N = normalize(input.normal);
        float3 V = normalize(-input.vwPos);

        float rim = 1.0f - saturate(dot(N, V));
        rim = pow(rim, max(rimParams.x, 0.001f));

        col.rgb += rimColor.rgb * rim * rimColor.a;
    }

    return col;
}