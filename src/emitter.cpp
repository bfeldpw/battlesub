#include "emitter.h"

std::mt19937 Emitter::Generator_;

void Emitter::emit()
{
    std::normal_distribution<float> DistAngle(Angle_, AngleStdDev_);
    std::normal_distribution<float> DistScale(Scale_, ScaleStdDev_);

    for (auto i=0; i<Number_; ++i)
    {
        GameObject* LandscapeDebris = GlobalFactories::Debris.create();
        if (LandscapeDebris != nullptr)
        {
            Color3 Col = {0.2f, 0.2f, 0.3f};
            if (Type_ == GameObjectTypeE::DEBRIS_SUBMARINE)
            {
                Col = {0.1f, 0.1f, 0.2f};
            }

            b2BodyDef BodyDef;
            BodyDef.type = b2_dynamicBody;
            BodyDef.active = true;
            BodyDef.angularDamping = 1.0f;
            BodyDef.linearDamping = 1.0f;
            BodyDef.position.Set(OriginX_+i*0.01f, OriginY_);
            BodyDef.linearVelocity.Set(std::cos(DistAngle(Generator_))*(Velocity_+VelocityStdDev_),
                                        std::sin(DistAngle(Generator_))*(Velocity_+VelocityStdDev_));
            BodyDef.angularVelocity = 0.5f*DistAngle(Generator_);
            LandscapeDebris->setColor(Col)
                            .setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON))
                            .setMeshes(GlobalResources::Get.getMeshes(Type_))
                            .setScene(GlobalResources::Get.getScene())
                            .setShader(GlobalResources::Get.getShader())
                            .setShapes(GlobalResources::Get.getShapes(Type_))
                            .setWorld(GlobalResources::Get.getWorld())
                            .init(Type_, BodyDef);

            b2Filter Filter;
            Filter.categoryBits = 0x0002;
            Filter.maskBits = 0xFFF9;
            LandscapeDebris->getBody()->GetFixtureList()->SetFilterData(Filter);
            auto Size = DistScale(Generator_);
            auto Asymmetry = 0.5f;
            if (Type_ == GameObjectTypeE::DEBRIS_SUBMARINE)
            {
                Size *= 0.5f;
                Asymmetry = 2.0f;
            }
            static_cast<GameObject*>(LandscapeDebris->getBody()->GetUserData())->setScale(Size, Size+Asymmetry*DistScale(Generator_));
        }
        else
        {
            GlobalErrorHandler.reportError("Couldn't create LandscapeDebris, out of pool memory.");
        }
    }
    Counter_ += Number_;
}
