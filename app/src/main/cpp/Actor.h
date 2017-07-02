#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <cassert>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <tinyxml2.h>

#include "LogWrapper.h"
#include "Texture.h"
#include "ResourceManager.h"

namespace Actors {
    class Actor;
    class ActorComponent;
    class ActorFactory;

    using ActorId = uint64_t;
    using ComponentId = std::string;

    using StrongActorPtr = std::shared_ptr<Actor>;
    using WeakActorPtr = std::weak_ptr<Actor>;
    using StrongActorComponentPtr = std::shared_ptr<ActorComponent>;

    using ActorComponentCreatorFn = std::function<ActorComponent*(void)>;

    using ActorComponents = std::unordered_map<ComponentId, StrongActorComponentPtr>;
    using ActorComponentCreatorMap = std::unordered_map<std::string, ActorComponentCreatorFn>;

    using XmlElement = tinyxml2::XMLElement;
    using XmlDocument = tinyxml2::XMLDocument;
    using tinyxml2::XMLError;

    template <class Type>
    std::shared_ptr<Type> MakeStrongPtr(std::weak_ptr<Type> pWeakPtr) {
        if (!pWeakPtr.expired())
            return std::shared_ptr<Type>(pWeakPtr);
        else
            return std::shared_ptr<Type>();
    }
}


namespace Actors {

    class Actor
    {
        friend class ActorFactory;

    public:
        explicit Actor(ActorId id);
        ~Actor();
        bool Init(XmlElement * pData);
        void PostInit();
        void Destroy();
        void Update(double deltaSec);

        ActorId GetId(void) const { return mId; }

        template<class ComponentType>
        std::weak_ptr<ComponentType> GetComponent(ComponentId id) {
            auto findIt = mComponents.find(id);
            if (findIt != mComponents.end()) {
                StrongActorComponentPtr pBase(findIt->second);
                // cast to subclass version of the pointer
                std::shared_ptr<ComponentType> pSub(
                        std::static_pointer_cast<ComponentType>(pBase));
                std::weak_ptr<ComponentType> pWeakSub(pSub); // convert strong pointer
                // to weak pointer
                return pWeakSub; // return the weak pointer
            }
            else {
                return std::weak_ptr<ComponentType>();
            }
        }

    private:
        // This is called by the ActorFactory; no one else should be
        // adding components.
        void AddComponent(StrongActorComponentPtr pComponent);

    private:
        ActorId         mId;
        std::string     mType;
        ActorComponents mComponents; // all components this actor has
    };


    class ActorComponent
    {
        friend class ActorFactory;

    public:
        virtual ~ActorComponent() { }

        virtual bool VInit(XmlElement* pData) = 0;
        virtual void VPostInit() { }
        virtual void VUpdate(double deltaSec) { }
        virtual ComponentId VGetId(void) const = 0;

    private:
        void SetOwner(StrongActorPtr pOwner) { mPtrOwner = pOwner; }

    protected:
        StrongActorPtr mPtrOwner;
    };



}