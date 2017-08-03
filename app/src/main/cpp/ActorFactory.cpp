
#include "ActorFactory.h"
#include "ActorComponents.h"

namespace Actors {

    ActorComponent * CreateRenderAnimationComponent() {
        return new RenderAnimationComponent;
    }

    ActorComponent * CreatePhysicsComponent() {
        return new PhysicsComponent;
    }

    ActorComponent * CreateRenderComponent() {
        return new RenderComponent;
    }

    ActorFactory::ActorFactory() : mLastActorId {} {
        {
            auto result = mActorComponentCreators.insert(
                    std::make_pair("RenderAnimationComponent", CreateRenderAnimationComponent));
            if (!result.second) {
                Log::error("Can't add creator for component");
                assert(result.second);
            }
        }

        {
            auto result = mActorComponentCreators.insert(
                    std::make_pair("RenderComponent", CreateRenderComponent));
            if (!result.second) {
                Log::error("Can't add creator for component");
                assert(result.second);
            }
        }

        {
            auto result = mActorComponentCreators.insert(
                    std::make_pair("PhysicsComponent", CreatePhysicsComponent));
            if (!result.second) {
                Log::error("Can't add creator for component");
                assert(result.second);
            }
        }
    }


    StrongActorPtr ActorFactory::CreateActor(std::string const &actorResource) {
        XmlDocument actorXml;
        std::vector<uint8_t> xmlBuffer;
        ResourceManager::Read(actorResource, xmlBuffer);
        auto result = actorXml.Parse(std::string(xmlBuffer.begin(), xmlBuffer.end()).c_str());

        Log::debug("LOAD ACTOR %s", actorResource.c_str());
        if (result != XMLError::XML_SUCCESS) {
            assert(result == XMLError::XML_SUCCESS);
            return StrongActorPtr {};
        }

        XmlElement *actorXmlRoot = actorXml.RootElement();
        // create the actor instance
        assert(actorXmlRoot);
        StrongActorPtr ptrActor{new Actor{GetNextActorId()}};
        if (!ptrActor->Init(actorXmlRoot)) {
            Log::error("Failed to initialize actor: %s", actorResource.c_str());
            assert(false);
            return StrongActorPtr();
        }

        // Loop through each child element and load the component
        for (XmlElement *pNode = actorXmlRoot->FirstChildElement(); pNode;
             pNode = pNode->NextSiblingElement()) {

            assert(pNode);
            StrongActorComponentPtr pComponent(CreateComponent(pNode));

            if (pComponent) {
                ptrActor->AddComponent(pComponent);
                pComponent->SetOwner(ptrActor);
            }
            else {
                Log::debug("Can't create component");
                assert(pComponent);
                return StrongActorPtr();
            }
        }

        // Now that the actor has been fully created, run the post init phase
        ptrActor->PostInit();
        Log::debug("LOAD ACTOR %s SUCCESS", actorResource.c_str());
        return ptrActor;
    }

    StrongActorComponentPtr ActorFactory::CreateComponent(XmlElement *pData) {
        std::string name(pData->Value());
        Log::debug("LOAD COMPONENT %s", name.c_str());

        StrongActorComponentPtr pComponent {nullptr};
        auto findIt = mActorComponentCreators.find(name);
        if (findIt != mActorComponentCreators.end()) {
            ActorComponentCreatorFn creator = findIt->second;
            pComponent.reset(creator());
        }
        else {
            Log::error("Couldn’t find ActorComponent named %s", name.c_str());
            assert(findIt != mActorComponentCreators.end());
            return StrongActorComponentPtr(); // fail
        }
        // initialize the component if we found one
        if (pComponent) {
            if (!pComponent->VInit(pData)) {
                Log::error("Component failed to initialize: %s", name.c_str());
                assert(false);
                return StrongActorComponentPtr();
            }
        }
        else {
            Log::error("Component is NULL: %s", name.c_str());
            assert(pComponent);
        }
        // pComponent will be NULL if the component wasn’t found. This isn’t
        // necessarily an error since you might have a custom CreateComponent()
        // function in a subclass.
        Log::debug("LOAD COMPONENT %s SUCCESS", name.c_str());
        return pComponent;
    }


}