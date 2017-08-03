
#include "ActorComponents.h"
#include "GLState.h"
#include <glm/gtx/projection.hpp>

namespace Actors {
    ComponentId const RenderAnimationComponent::COMPONENT_ID {"RenderAnimationComponent"};
    ComponentId const RenderComponent::COMPONENT_ID          {"RenderComponent"};
    ComponentId const PhysicsComponent::COMPONENT_ID         {"PhysicsComponent"};
}

namespace Actors {
    Actors::RenderAnimationComponent::RenderAnimationComponent() : mAnimationTime{},
                                                                   mTimeCollector{},
                                                                   mFramesCount{}
    {}

    bool Actors::RenderAnimationComponent::VInit(Actors::XmlElement *pData)  {
        mAnimationTime = pData->FloatAttribute("AnimationTime");
        Log::debug("Animation time %f", mAnimationTime);
        assert(mAnimationTime > 0.f);

        std::vector<std::string> texNames;
        std::string texNamesFromXml = pData->Attribute("Textures");
        ParseTextureNamesFromXml(texNamesFromXml, texNames);

        assert(texNames.size() > 0);
        for(auto const & texName: texNames) {
            Log::debug("Parsed name = %s", texName.c_str());
            mTextures.push_back(ResourceManager::GetTexture(texName));
        }

        mItCurrentFrame = mTextures.begin();
        assert(mTextures.size() == texNames.size());
        mFramesCount = mTextures.size();

        return true;
    }

    void RenderAnimationComponent::VUpdate(double deltaSec) {
        mTimeCollector += deltaSec;
        if(mTimeCollector > mAnimationTime) {
            ++mItCurrentFrame;
            mTimeCollector = 0.f;
            if(mItCurrentFrame == mTextures.end()) mItCurrentFrame = mTextures.begin();
        }
    }

    void RenderAnimationComponent::ParseTextureNamesFromXml(std::string const &src,
                                                            std::vector<std::string> &dst) {
        std::string texName;
        for(auto const & c: src) {
            if(!ispunct(c))
                texName.push_back(c);
            else {
                dst.push_back(texName);
                texName.clear();
            }
        }
    }





    RenderComponent::RenderComponent() : mPtrTexture{nullptr}
    {}

    bool RenderComponent::VInit(XmlElement *pData) {
        assert(pData);
        std::string texName = pData->Attribute("Texture");
        mPtrTexture = ResourceManager::GetTexture(texName);

        return true;
    }








    PhysicsComponent::PhysicsComponent() : mPosition{},
                                           mSize{},
                                           mDegrees{},
                                           mVelocity{},
                                           mAcceleration{},
                                           mForce{},
                                           mPositionCenter{}
    {}

    bool PhysicsComponent::VInit(XmlElement *pData) {
        Log::debug("Init Physics component");
        assert(pData);

        std::string type = pData->Attribute("Type");
        mType = ParsePhysicsComponentType(type);

        int32_t scaleX = pData->IntAttribute("ScaleX");
        int32_t scaleY = pData->IntAttribute("ScaleY");
        int32_t scaleMaxX = pData->IntAttribute("ScaleMaxX");
        int32_t scaleMaxY = pData->IntAttribute("ScaleMaxY");

        assert(scaleX <= scaleMaxX && scaleX >= 0 && scaleMaxX >= 0);
        assert(scaleY <= scaleMaxY && scaleY >= 0 && scaleMaxY >= 0);

        GLfloat posX = scaleX == 0 || scaleMaxX == 0 ? 0.f : static_cast<GLfloat>(GLState::GetInstance().GetScreenWidth()) / scaleMaxX * scaleX;
        GLfloat posY = scaleY == 0 || scaleMaxX == 0 ? 0.f : static_cast<GLfloat>(GLState::GetInstance().GetScreenHeight()) / scaleMaxY * scaleY;
        mPosition = {posX, posY};
        Log::debug("Position: x = %f, y = %f", posX, posY);

        int32_t scaleSizeX = pData->IntAttribute("ScaleSizeX");
        int32_t scaleSizeY = pData->IntAttribute("ScaleSizeY");
        int32_t scaleMaxSizeX = pData->IntAttribute("ScaleMaxSizeX");
        int32_t scaleMaxSizeY = pData->IntAttribute("ScaleMaxSizeY");

        GLfloat sizeX = scaleSizeX == 0 || scaleMaxSizeX == 0 ? 0.f : static_cast<GLfloat>(GLState::GetInstance().GetScreenWidth()) / scaleMaxSizeX * scaleSizeX;
        GLfloat sizeY = scaleSizeY == 0 || scaleMaxSizeY == 0 ? 0.f : static_cast<GLfloat>(GLState::GetInstance().GetScreenHeight()) / scaleMaxSizeY * scaleSizeY;
        mSize = {sizeX, sizeY};

        Log::debug("Size X = %f, Y = %f", sizeX, sizeY);

        mVelocity.x = pData->FloatAttribute("VelocityX");
        mVelocity.y = pData->FloatAttribute("VelocityY");
        mAcceleration.x = pData->FloatAttribute("AccelerationX");
        mAcceleration.y = pData->FloatAttribute("AccelerationY");

        mPositionCenter = pData->BoolAttribute("PositionCenter");

        Log::debug("Velocity = %f %f", mVelocity.x, mVelocity.y);
        Log::debug("Acceleration = %f %f", mAcceleration.x, mAcceleration.y);

        return true;
    }

    void PhysicsComponent::VPostInit() {
        assert(mPtrOwner);

        auto pWeakRenderAnimationComponent = mPtrOwner->GetComponent<Actors::RenderAnimationComponent>("RenderAnimationComponent");
        auto pStrongRenderAnimationComponent = Actors::MakeStrongPtr(pWeakRenderAnimationComponent);

        std::shared_ptr<Texture> ptrTexture {nullptr};
        if(pStrongRenderAnimationComponent) {
            ptrTexture = pStrongRenderAnimationComponent->GetCurrentFrameTexture();
        }
        else {
            auto pWeakRenderComponent = mPtrOwner->GetComponent<Actors::RenderComponent>("RenderComponent");
            auto pStrongRenderComponent = Actors::MakeStrongPtr(pWeakRenderComponent);

            assert(pStrongRenderComponent);
            ptrTexture = pStrongRenderComponent->GetTexture();
        }

        assert(ptrTexture);

        if(mSize.x > 0.f || mSize.y > 0.f) {
            if(mSize.y > 0.f) {
                float scaleY = mSize.y / ptrTexture->GetHeight() ;
                mSize.x = ptrTexture->GetWidth() * scaleY;
            }
            else {
                float scaleX = mSize.x / ptrTexture->GetWidth();
                mSize.y = ptrTexture->GetHeight() * scaleX;
            }
        }
        else {
            mSize.x = ptrTexture->GetWidth();
            mSize.y = ptrTexture->GetHeight();
        }

        Log::debug("Final size %f %f", mSize.x, mSize.y);

        if(!mPositionCenter) return;

        mPosition.x -= mSize.x / 2.f;
        mPosition.y -= mSize.y / 2.f;

        Log::debug("Position center %f %f", mPosition.x, mPosition.y);

    }

    void PhysicsComponent::VUpdate(double deltaSec) {
        auto dt = static_cast<float>(deltaSec);

        mVelocity.y += mAcceleration.y * dt;
        mPosition.y += mVelocity.y * dt;

        mVelocity.x += mAcceleration.x * dt;
        mPosition.x += mVelocity.x * dt;
    }

    bool PhysicsComponent::CheckCollision(PhysicsComponent const & other) {
        assert(this->mType == PhysicsComponentType::CIRLCE && other.mType == PhysicsComponentType::RECTANGLE);

        glm::vec2 corners [4];
        corners[0].x = other.mPosition.x;
        corners[0].y = other.mPosition.y;
        corners[1].x = corners[0].x + other.mSize.x;
        corners[1].y = corners[0].y;
        corners[2].x = corners[1].x;
        corners[2].y = corners[1].y + other.mSize.y;
        corners[3].x = corners[0].x;
        corners[3].y = corners[0].y + other.mSize.y;

        auto edge1 = glm::vec2(corners[1].x - corners[0].x,
                               corners[1].y - corners[0].y);
        edge1 = glm::normalize(edge1);

        auto edge3 = glm::vec2(corners[3].x - corners[0].x,
                               corners[3].y - corners[0].y);
        edge3 = glm::normalize(edge3);

        float proj = glm::dot(corners[0], edge1);
        float edge1min = proj;
        float edge1max = proj;

        proj = glm::dot(corners[1], edge1);

        edge1min = proj < edge1min ? proj : edge1min;
        edge1max = proj > edge1max ? proj : edge1max;

        proj = glm::dot(corners[0], edge3);
        auto edge3min = proj;
        auto edge3max = proj;

        proj = glm::dot(corners[3], edge3);

        edge3min = proj < edge3min ? proj : edge3min;
        edge3max = proj > edge3max ? proj : edge3max;

        auto circleCenter = glm::vec2(this->mPosition.x + mSize.x / 2.f, this->mPosition.y + mSize.y / 2.f);
        float radii = mSize.y / 2.f;

        float projCircleCenter1 = glm::dot(circleCenter, edge1);
        auto min01 = projCircleCenter1 - radii;
        auto max01 = projCircleCenter1 + radii;

//        Log::debug("min01 %f, edge1max %f ||| max01 %f, edge1min %f", min01, edge1max, max01, edge1min);
        if(min01 > edge1max || max01 < edge1min)
            return false;

        float projCircleCenter3 = glm::dot(circleCenter, edge3);
        auto min03 = projCircleCenter3 - radii;
        auto max03 = projCircleCenter3 + radii;

//        Log::debug("min03 %f, edge3max %f ||| max03 %f, edge3min %f", min03, edge3max, max03, edge3min);
        if(min03 > edge3max || max03 < edge3min)
            return false;

        if(projCircleCenter1 < edge1min && projCircleCenter3 < edge3min)
            return CollideCornerCircle(corners[0], circleCenter, radii);

        if(projCircleCenter1 > edge1max && projCircleCenter3 < edge3min)
            return CollideCornerCircle(corners[1], circleCenter, radii);

        if(projCircleCenter1 > edge1max && projCircleCenter3 > edge3max)
            return CollideCornerCircle(corners[2], circleCenter, radii);

        if(projCircleCenter1 < edge1min && projCircleCenter3 > edge3max)
            return CollideCornerCircle(corners[3], circleCenter, radii);

        // Circle not in voronoi region so it is colliding with edge of box
        return true;
    }

    bool PhysicsComponent::CollideCornerCircle(glm::vec2 const &corner, glm::vec2 const & circleCenter, float radii) {
        auto diffVecSquared = corner - circleCenter; // Corner - circle
        diffVecSquared.x *= diffVecSquared.x;
        diffVecSquared.y *= diffVecSquared.y;
        // Calculate the sum of the radii, then square it
        auto RadiiSquared = radii * radii;
        // If corner and circle are colliding
        return (diffVecSquared.x + diffVecSquared.y <= RadiiSquared);
    }

    PhysicsComponentType PhysicsComponent::ParsePhysicsComponentType(std::string const & type) {
        if(type == "Rectangle") return PhysicsComponentType::RECTANGLE;
        if(type == "Circle") return PhysicsComponentType::CIRLCE;

        Log::error("WRONG PHYSICS_COMPONENT_TYPE %s", type.c_str());
        assert(false);
    }


}