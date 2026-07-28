#include "Grass.h"
#include <random>
#include "../../../Object/Collider/ColliderBase.h"
#include "../../../Object/Collider/ColliderModel.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Renderer/VertexMaterial.h"
#include "../../../Renderer/VertexRenderer.h"

Grass::Grass()
    :
    ActorBase()
{
}

Grass::~Grass()
{
    delete renderer_;
    renderer_ = nullptr;

    delete material_;
    material_ = nullptr;
}

void Grass::Update()
{
    time_ += 1.0f / 60.0f;
}

void Grass::Draw()
{
    material_->SetConstBuf(1,
        {
            time_,
            windPower_,
            windSpeed_,
            bendStrength_
        });

    material_->SetConstBuf(2,
        {
            playerPos_.x,
            playerPos_.y,
            playerPos_.z,
            bendRadius_
        });

    for (const auto& grass : grasses_)
    {
        drawTransform_.pos = grass.pos;

        drawTransform_.scl = VGet(
            grass.scale,
            grass.scale,
            grass.scale);

        drawTransform_.quaRot = Quaternion::Identity();
        drawTransform_.quaRotLocal = Quaternion::Identity();

        drawTransform_.Update();

        renderer_->Draw(drawTransform_.modelId);
    }
}

void Grass::InitLoad()
{
    drawTransform_.SetModel(
        resMng_.Load(ResourceManager::SRC::GRASS).handleId_);


    material_ = new VertexMaterial("VertexGrass.cso", "Pixelshader.cso", 3);

    auto dir = GetLightDirection();

    material_->AddConstBuf({ dir.x,dir.y, dir.z,0.0f });

    material_->AddConstBuf({
            time_,          // time
            windPower_,     // power
            windSpeed_,     // speed
            bendStrength_   // w: ‰Ÿ‚µ“|‚µ‚Ì‹­‚³
        });

    material_->AddConstBuf({
            playerPos_.x,
            playerPos_.y,
            playerPos_.z,
            bendRadius_
        });

    renderer_ = new VertexRenderer(*material_);
}

void Grass::InitTransform()
{
    drawTransform_.quaRot = Quaternion::Identity();
    drawTransform_.quaRotLocal = Quaternion::Identity();

    drawTransform_.scl = VGet(1.0f, 1.0f, 1.0f);
    drawTransform_.pos = VGet(0.0f, 0.0f, 0.0f);

    drawTransform_.Update();
}

void Grass::InitCollider()
{
}

void Grass::InitAnimation()
{
}

void Grass::InitPost()
{
}

void Grass::AddGrass(const VECTOR& pos, float scale)
{
    GrassData data;

    data.pos = pos;
    data.scale = scale;

    grasses_.push_back(data);
}

void Grass::AddHitCollider(ColliderModel* collider)
{
    stageCollider_ = collider;
}

void Grass::GenerateField(int count, float rangeXZ)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> distXZ(-rangeXZ, rangeXZ);
    std::uniform_real_distribution<float> distScale(0.8f, 1.2f);

    for (int i = 0; i < count; i++)
    {
        float x = distXZ(mt);
        float z = distXZ(mt);

        VECTOR pos = VGet(x, 3000.0f, z);

        if (!GetGroundPosition(pos))
        {
            continue;
        }

        AddGrass(pos, distScale(mt));
    }
}

bool Grass::GetGroundPosition(VECTOR& pos)
{
    if (stageCollider_ == nullptr)
    {
        return false;
    }

    VECTOR start = VAdd(pos, VGet(0.0f, 10000.0f, 0.0f));
    VECTOR end = VAdd(pos, VGet(0.0f, -10000.0f, 0.0f));

    MV1_COLL_RESULT_POLY hit =
        stageCollider_->GetNearestHitPolyLine(start, end);

    if (hit.HitFlag == FALSE)
    {
        return false;
    }

    pos = hit.HitPosition;
    return true;
}
