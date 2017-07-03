
#include "ActorComponents.h"
#include "GLContextWrapper.h"

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
        LogWrapper::debug("Animation time %f", mAnimationTime);
        assert(mAnimationTime > 0.f);

        std::vector<std::string> texNames;
        std::string texNamesFromXml = pData->Attribute("Textures");
        ParseTextureNamesFromXml(texNamesFromXml, texNames);

        assert(texNames.size() > 0);
        for(auto const & texName: texNames) {
            LogWrapper::debug("Parsed name = %s", texName.c_str());
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
        LogWrapper::debug("Init Physics component");
        assert(pData);

        int32_t scaleX = pData->IntAttribute("ScaleX");
        int32_t scaleY = pData->IntAttribute("ScaleY");
        int32_t scaleMaxX = pData->IntAttribute("ScaleMaxX");
        int32_t scaleMaxY = pData->IntAttribute("ScaleMaxY");

        assert(scaleX <= scaleMaxX && scaleX >= 0 && scaleMaxX >= 0);
        assert(scaleY <= scaleMaxY && scaleY >= 0 && scaleMaxY >= 0);

        GLfloat posX = scaleX == 0 || scaleMaxX == 0 ? 0.f : static_cast<GLfloat>(GLContextWrapper::GetInstance().GetScreenWidth()) / scaleMaxX * scaleX;
        GLfloat posY = scaleY == 0 || scaleMaxX == 0 ? 0.f : static_cast<GLfloat>(GLContextWrapper::GetInstance().GetScreenHeight()) / scaleMaxY * scaleY;
        mPosition = {posX, posY};
        LogWrapper::debug("Position: x = %f, y = %f", posX, posY);

        int32_t scaleSizeX = pData->IntAttribute("ScaleSizeX");
        int32_t scaleSizeY = pData->IntAttribute("ScaleSizeY");
        int32_t scaleMaxSizeX = pData->IntAttribute("ScaleMaxSizeX");
        int32_t scaleMaxSizeY = pData->IntAttribute("ScaleMaxSizeY");

        GLfloat sizeX = scaleSizeX == 0 || scaleMaxSizeX == 0 ? 0.f : static_cast<GLfloat>(GLContextWrapper::GetInstance().GetScreenWidth()) / scaleMaxSizeX * scaleSizeX;
        GLfloat sizeY = scaleSizeY == 0 || scaleMaxSizeY == 0 ? 0.f : static_cast<GLfloat>(GLContextWrapper::GetInstance().GetScreenHeight()) / scaleMaxSizeY * scaleSizeY;
        mSize = {sizeX, sizeY};

        LogWrapper::debug("Size X = %f, Y = %f", sizeX, sizeY);

        mVelocity.x = pData->FloatAttribute("VelocityX");
        mVelocity.y = pData->FloatAttribute("VelocityY");
        mAcceleration.x = pData->FloatAttribute("AccelerationX");
        mAcceleration.y = pData->FloatAttribute("AccelerationY");

        mPositionCenter = pData->BoolAttribute("PositionCenter");

        LogWrapper::debug("Velocity = %f %f", mVelocity.x, mVelocity.y);
        LogWrapper::debug("Acceleration = %f %f", mAcceleration.x, mAcceleration.y);

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

        LogWrapper::debug("Final size %f %f", mSize.x, mSize.y);

        if(!mPositionCenter) return;

        mPosition.x -= mSize.x / 2.f;
        mPosition.y -= mSize.y / 2.f;

        LogWrapper::debug("Position center %f %f", mPosition.x, mPosition.y);

    }

    void PhysicsComponent::VUpdate(double deltaSec) {
        auto dt = static_cast<float>(deltaSec);

        mVelocity.y += mAcceleration.y * dt;
        mPosition.y += mVelocity.y * dt;

        mVelocity.x += mAcceleration.x * dt;
        mPosition.x += mVelocity.x * dt;
    }


}