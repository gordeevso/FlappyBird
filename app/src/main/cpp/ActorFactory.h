#pragma once


#include "Actor.h"
#include "ActorComponents.h"

namespace Actors {

    class ActorFactory {
    public:
        ActorFactory();
        StrongActorPtr CreateActor(std::string const &actorResource);

    protected:
        StrongActorComponentPtr CreateComponent(XmlElement *pData);

    private:
        ActorId GetNextActorId() { return ++mLastActorId; }


    protected:
        ActorComponentCreatorMap mActorComponentCreators;

    private:
        ActorId mLastActorId;
    };
}