#pragma once

#include "Actor.h"

namespace Actors {

    class RenderAnimationComponent : public ActorComponent
    {
    public:
        RenderAnimationComponent();

        virtual ComponentId VGetId() const { return COMPONENT_ID; }
        std::shared_ptr<Texture> const GetCurrentFrameTexture() const { return *mItCurrentFrame; }

        virtual bool VInit(XmlElement * pData);
        virtual void VPostInit();
        virtual void VUpdate(double deltaSec);

    private:
        void ParseTextureNamesFromXml(std::string const & src, std::vector<std::string> & dst);

    private:
        std::vector<std::shared_ptr<Texture>> mTextures;
        std::vector<std::shared_ptr<Texture>>::const_iterator mItCurrentFrame;
        float mAnimationTime;
        int32_t mFramesCount;
        float mTimeCollector;

        static ComponentId const COMPONENT_ID;

    };



    class PhysicsComponent : public ActorComponent {
    public:
        PhysicsComponent();

        virtual ComponentId VGetId() const { return COMPONENT_ID; }

        virtual bool VInit(XmlElement * pData);
        virtual void VPostInit();
        virtual void VUpdate(double deltaSec);
        void SetVelocityDefault() { mVelocity = mVelocityDefault; }

        void SetPosition(glm::vec2 const & pos) { mPosition = pos; }
        void SetSize(glm::vec2 const & size) { mSize = size; }
        void SetRotation(GLfloat degrees) { mDegrees = degrees; }
        void SetVelocity(glm::vec2 const & velocity) { mVelocity = velocity; }
        void SetAcceleration(glm::vec2 const & acceleration) { mAcceleration = acceleration; }

        glm::vec2 const & GetPosition() const { return mPosition; }
        glm::vec2 const & GetSize() const { return mSize; }
        GLfloat GetRotation() const { return mDegrees; }
        glm::vec2 const & GetVelocity() const { return mVelocity; }
        glm::vec2 const & GetAcceleration() const { return mAcceleration; }

    private:
        glm::vec2 mPosition;
        glm::vec2 mSize;
        GLfloat   mDegrees;
        glm::vec2 mVelocity;
        glm::vec2 mAcceleration;
        glm::vec2 mForce;

        glm::vec2 mVelocityDefault;
        static ComponentId const COMPONENT_ID;
    };
}
