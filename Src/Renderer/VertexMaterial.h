#pragma once
#include <string>
#include <vector>
#include <DxLib.h>

class VertexMaterial
{
public:
    //コンストラクタ
    VertexMaterial(std::string shaderFileNameVS, std::string shaderFileNamePS, int constBufFloat4Size);

    //デストラクタ
    ~VertexMaterial(void);

    void AddConstBuf(const FLOAT4& constBuf);
    void SetConstBuf(int idx, const FLOAT4& constBuf);

    int GetVS(void) const;
    int GetPS(void) const;
    int GetConstBuf(void) const;
    const std::vector<FLOAT4>& GetConstBufs(void) const;

private:

    int shaderVS_;
    int shaderPS_;
    int constBufFloat4Size_;
    int constBuf_;
    std::vector<FLOAT4> constBufs_;
};



