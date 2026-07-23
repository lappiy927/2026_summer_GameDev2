// VS/PS共通
#include "../Common/VertexToPixelHeader.hlsli"

// IN
#include "../Common/Vertex/VertexInputType.hlsli"
#define VERTEX_INPUT DX_MV1_VERTEX_TYPE_NMAP_1FRAME

// OUT
#define VS_OUTPUT VertexToPixelLit
#include "../Common/Vertex/VertexShader3DHeader.hlsli"

//--------------------------------------------
// Grass Constant Buffer
//--------------------------------------------
cbuffer GrassBuffer : register(b8)
{
    float4 lightDir; // 未使用（オフセット合わせ用）
    float4 grassParam; // x=time, y=power, z=speed, w=bendStrength
    float4 playerParam; // xyz=playerPos(world), w=bendRadius
};

//=========================================================

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT ret;

    //--------------------------------------------
    // Local Position
    //--------------------------------------------

    float4 lLocalPosition;
    lLocalPosition.xyz = VSInput.pos;
    lLocalPosition.w = 1.0f;

    //--------------------------------------------
    // Root Weight
    //--------------------------------------------

    // UV下側=0
    // UV上側=1
    float weight = saturate(1.0f - VSInput.uv0.y);

    //--------------------------------------------
    // インスタンス位置 / スケールを取得
    // （回転は入っていない前提。位置は平行移動成分、
    //   スケールはX軸基底ベクトルの長さから求める）
    //--------------------------------------------

    float3 instancePos = float3(
        g_base.localWorldMatrix._41,
        g_base.localWorldMatrix._42,
        g_base.localWorldMatrix._43);

    float instanceScale = length(float3(
        g_base.localWorldMatrix._11,
        g_base.localWorldMatrix._12,
        g_base.localWorldMatrix._13));

    //--------------------------------------------
    // カメラ空間の軸を viewMatrix から取得
    // （ビルボード用）
    //--------------------------------------------

    float3 cameraRight = float3(
        g_base.viewMatrix._11,
        g_base.viewMatrix._21,
        g_base.viewMatrix._31);

    float3 cameraUp = float3(
        g_base.viewMatrix._12,
        g_base.viewMatrix._22,
        g_base.viewMatrix._32);

    float3 cameraForward = float3(
        g_base.viewMatrix._13,
        g_base.viewMatrix._23,
        g_base.viewMatrix._33);

    //--------------------------------------------
    // Wind
    //--------------------------------------------

    float time = grassParam.x;
    float power = grassParam.y;
    float speed = grassParam.z;
    float bendStrength = grassParam.w;

    float phase =
        instancePos.x * 0.04f +
        instancePos.z * 0.03f;

    float sway =
        sin(time * speed + phase);

    //--------------------------------------------
    // Player Push（プレイヤーが近づいたら押し倒す）
    //--------------------------------------------

    float3 toGrass = instancePos - playerParam.xyz;
    toGrass.y = 0.0f; // 水平方向だけで判定・押し出し

    float distToPlayer = length(toGrass);

    float3 pushDir =
        (distToPlayer > 0.0001f)
            ? (toGrass / distToPlayer)
            : float3(1.0f, 0.0f, 0.0f);

    float bendRadius = playerParam.w;

    // プレイヤーに近いほど1、離れると0
    float pushFalloff =
        saturate(1.0f - (distToPlayer / bendRadius));

    // 急激に立ち上がらないよう滑らかにする（お好みで）
    pushFalloff = pushFalloff * pushFalloff;

    float pushAmount =
        pushFalloff * bendStrength * weight;

    //--------------------------------------------
    // ビルボードオフセット
    //--------------------------------------------

    float3 billboardOffset =
        cameraRight * lLocalPosition.x * instanceScale +
        cameraUp * lLocalPosition.y * instanceScale;

    billboardOffset +=
        cameraRight * sway * power * weight;

    //--------------------------------------------
    // World Position
    //--------------------------------------------

    float4 lWorldPosition;
    lWorldPosition.xyz =
        instancePos + billboardOffset + pushDir * pushAmount;
    lWorldPosition.w = 1.0f;

    ret.worldPos.xyz = lWorldPosition.xyz;

    //--------------------------------------------
    // World → View
    //--------------------------------------------

    float4 lViewPosition;

    lViewPosition.xyz =
        mul(lWorldPosition,
            g_base.viewMatrix);

    lViewPosition.w = 1.0f;

    ret.vwPos.xyz = lViewPosition.xyz;

    //--------------------------------------------
    // Projection
    //--------------------------------------------

    ret.svPos =
        mul(lViewPosition,
            g_base.projectionMatrix);

    //--------------------------------------------
    // UV
    //--------------------------------------------

    ret.uv = VSInput.uv0;

    //--------------------------------------------
    // Normal
    // ビルボードなので常にカメラの方を向く
    //--------------------------------------------

    ret.normal = -cameraForward;

    //--------------------------------------------
    // Diffuse
    //--------------------------------------------

    ret.diffuse = VSInput.diffuse;

    //--------------------------------------------
    // Light
    //--------------------------------------------

    ret.lightDir = float3(0, 0, 0);
    ret.lightAtPos = float3(0, 0, 0);

    return ret;
}