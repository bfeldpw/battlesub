#ifndef EMITTER_H
#define EMITTER_H

#include <random>

#include <Box2D/Box2D.h>

#include "entity.h"
#include "game_object.h"
#include "global_factories.h"
#include "global_resources.h"

class Emitter : public Entity
{
    
    public:
    
        void destroy() {}
        void emit()
        {
            std::normal_distribution<float> DistAngle(Angle_, AngleStdDev_);
            std::normal_distribution<float> DistScale(1.5f, 1.3f);
            
            for (auto i=0; i<Number_; ++i)
            {
                GameObject* LandscapeDebris = GlobalFactories::Debris.create();
                b2BodyDef BodyDef;
                BodyDef.type = b2_dynamicBody;
                BodyDef.active = true;
                BodyDef.angularDamping = 1.0f;
                BodyDef.linearDamping = 1.0f;
                BodyDef.position.Set(OriginX_+i*0.01f, OriginY_);
                BodyDef.linearVelocity.Set(std::cos(DistAngle(Generator_))*(Velocity_+VelocityStdDev_),
                                           std::sin(DistAngle(Generator_))*(Velocity_+VelocityStdDev_));
                BodyDef.angularVelocity = 0.5f*DistAngle(Generator_);
                LandscapeDebris->setColor({0.2f, 0.2f, 0.3f})
                                .setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON))
                                .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::DEBRIS))
                                .setScene(GlobalResources::Get.getScene())
                                .setShader(GlobalResources::Get.getShader())
                                .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::DEBRIS))
                                .setWorld(GlobalResources::Get.getWorld())
                                .init(GameObjectTypeE::DEBRIS, BodyDef);
                
                b2Filter Filter;
                Filter.categoryBits = 0x0002;
                Filter.maskBits = 0xFFFD;
                LandscapeDebris->getBody()->GetFixtureList()->SetFilterData(Filter);
                auto Size = DistScale(Generator_);
                static_cast<GameObject*>(LandscapeDebris->getBody()->GetUserData())->setScale(Size, Size+0.5f*DistScale(Generator_));
            }
            Counter_ += Number_;
        }
        bool isFinished() {return Counter_ >= Number_;}
        void setFrequency(float Frequency) {Frequency_ = Frequency;}
        
        Emitter& setAngle(float a)
        {
            Angle_ = a;
            return *this;
        }
        Emitter& setAngleStdDev(float as)
        {
            AngleStdDev_ = as;
            return *this;
        }
        Emitter& setOrigin(float x, float y)
        {
            OriginX_ = x;
            OriginY_ = y;
            return *this;
        }
        Emitter& setVelocity(float v)
        {
            Velocity_ = v;
            return *this;
        }
        Emitter& setVelocityStdDev(float vs)
        {
            VelocityStdDev_ = vs;
            return *this;
        }
    
    private:

        std::mt19937 Generator_;
        
        float  Angle_ = 0.0f;
        float  AngleStdDev_ = 0.0f;
        float  Frequency_ = 0.0f;
        int    Counter_ = 0;
        int    Number_ = 10;
        float  OriginX_ = 0.0f;
        float  OriginY_ = 0.0f;
        float  Velocity_ = 1.0f;
        float  VelocityStdDev_ = 0.0f;
};

#endif // EMITTER_H
