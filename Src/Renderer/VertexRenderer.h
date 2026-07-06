#pragma once
#include<DxLib.h>
#include"VertexMaterial.h"

class VertexRenderer
{
public:

	static constexpr int CONSTANT_BUF_SLOT_BEGIN_VS = 8;

	VertexRenderer(VertexMaterial& material);
	~VertexRenderer(void);

	void Draw(int modelId);

private:
	VertexMaterial& material_;
};