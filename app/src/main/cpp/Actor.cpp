
#include "Actor.h"
#include "GLContextWrapper.h"

namespace Actors {

    Actor::Actor(ActorId id) {
        mId = id;
        mType = "Unknown";
    }

    Actor::~Actor() {
        LogWrapper::debug("Destroying actor %d", mId);

        assert(mComponents.empty());
    }

    bool Actor::Init(XmlElement* pData) {
//        GCC_LOG("Actor", std::string("Initializing Actor ") + ToStr(mId));
        mType = pData->Attribute("Type");
        LogWrapper::debug("Actor Type %s", mType.c_str());

        return true;
    }

    void Actor::PostInit() {
        for (auto const & component: mComponents) {
            component.second->VPostInit();
        }
    }

    void Actor::Destroy() {
        mComponents.clear();
    }

    void Actor::Update(double deltaSec) {
        for (auto const & component: mComponents) {
            component.second->VUpdate(deltaSec);
        }
    }


    void Actor::AddComponent(StrongActorComponentPtr pComponent) {
        auto success = mComponents.insert(std::make_pair(pComponent->VGetId(), pComponent));
        assert(success.second);
    }





}