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
            
            for (auto i=0u; i<Number_; ++i)
            {
                Debris* LandscapeDebris = GlobalFactories::Debris.create();
                b2BodyDef BodyDef;
                BodyDef.type = b2_dynamicBody;
                BodyDef.active = true;
                BodyDef.angularDamping = 5.0f;
                BodyDef.linearDamping = 1.0f;
                BodyDef.position.Set(OriginX_+i*0.01f, OriginY_);
                BodyDef.linearVelocity.Set(std::cos(DistAngle(Generator_))*(Velocity_+VelocityStdDev_),
                                           std::sin(DistAngle(Generator_))*(Velocity_+VelocityStdDev_));
                LandscapeDebris->setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::DEBRIS))
                                .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::DEBRIS))
                                .setShader(GlobalResources::Get.getShader());
                LandscapeDebris->init(GlobalResources::Get.getWorld(), GlobalResources::Get.getScene(),
                                      BodyDef, GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
                
                b2Filter Filter;
                Filter.categoryBits = 0x0002;
                Filter.maskBits = 0xFFFD;
                LandscapeDebris->getBody()->GetFixtureList()->SetFilterData(Filter);
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
