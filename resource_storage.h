#ifndef RESOURCE_STORAGE_H
#define RESOURCE_STORAGE_H

#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Trade/MeshData2D.h>

#include "game_object.h"

class ResourceStorage
{
    public:
        
        void init()
        {
            {
                std::vector<b2Vec2> Verts;
                Verts.push_back({-500.0f,  300.0f});
                Verts.push_back({ 500.0f,  300.0f});
                Verts.push_back({ 500.0f, -300.0f});
                Verts.push_back({-500.0f, -300.0f});
                
                GL::Mesh Mesh;
                GL::Buffer Buffer;
                Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Verts), GL::BufferUsage::StaticDraw);
                Mesh.setCount(Verts.size())
                    .setPrimitive(GL::MeshPrimitive::LineLoop)
                    .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
                MeshLandscape_ = std::move(Mesh);
            }
            {
                std::vector<b2Vec2> Verts;
                Verts.push_back({-0.02f, 0.0f});
                Verts.push_back({ 0.02f, 0.0f});
                Verts.push_back({ 0.02f, 0.1f});
                Verts.push_back({ 0.0f,  0.15f});
                Verts.push_back({-0.02f, 0.1f});
                
                GL::Mesh Mesh;
                GL::Buffer Buffer;
                Buffer.setData(GameObject::convertGeometryPhysicsToGraphics(Verts), GL::BufferUsage::StaticDraw);
                Mesh.setCount(Verts.size())
                    .setPrimitive(GL::MeshPrimitive::TriangleFan)
                    .addVertexBuffer(std::move(Buffer), 0, Shaders::VertexColor2D::Position{});
                MeshProjectile_ = std::move(Mesh);
            }
            
            MeshSub_ = MeshTools::compile(Primitives::squareSolid());
            
            IsInitialised = true;
        }
        
        GL::Mesh* getMeshLandscape() {assert(IsInitialised == true); return &MeshLandscape_;}
        GL::Mesh* getMeshProjectile() {assert(IsInitialised == true); return &MeshProjectile_;}
        GL::Mesh* getMeshSub() {assert(IsInitialised == true); return &MeshSub_;}

    private:

        bool IsInitialised = false;
        GL::Mesh MeshLandscape_{NoCreate};
        GL::Mesh MeshProjectile_{NoCreate};
        GL::Mesh MeshSub_{NoCreate};
        
    public:
        
        static ResourceStorage Global;
        
};

#endif // RESOURCE_STORAGE_H
