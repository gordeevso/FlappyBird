#include "Scene.h"
#include <cmath>

Scene::Scene() : mPtrBird {nullptr},
                 mPtrActorFactory {new Actors::ActorFactory},
                 mPtrSpriteRenderer {new SpriteRenderer},
                 mTargetTapDistance{},
                 mTargetTapTime{},
                 mCheckInputTap{false},
                 mCurrentState{OwlState::TAP}
{
    tinyxml2::XMLDocument sceneXml;
    std::vector<uint8_t> xmlBuffer;
    ResourceManager::Read("xmlSettings/scene.xml", xmlBuffer);
    auto result = sceneXml.Parse(std::string(xmlBuffer.begin(), xmlBuffer.end()).c_str());

    LogWrapper::debug("LOAD Settings xmlSettings/scene.xml");
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        assert(result == tinyxml2::XMLError::XML_SUCCESS);
    }

    tinyxml2::XMLElement *sceneXmlRoot = sceneXml.RootElement();
    mTargetTapDistance = sceneXmlRoot->FloatAttribute("TargetTapDistance");
    mTargetTapTime = sceneXmlRoot->FloatAttribute("TargetTapTime");

    mPtrBird = mPtrActorFactory->CreateActor("xmlSettings/owl.xml");
    auto ptrWeakPhysicsComponent = mPtrBird->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
    mPtrBirdPhysics = Actors::MakeStrongPtr(ptrWeakPhysicsComponent);
}

void Scene::Update(double deltaSec) {
    auto dt = static_cast<float>(deltaSec);

    if(mCheckInputTap) mCurrentState = OwlState::TAP;

    switch(mCurrentState) {
        case OwlState::DEFAULT: {
//            LogWrapper::debug("STATE DEFAULT");
//            mPtrBirdPhysics->SetVelocityDefault();
            break;
        }

        case OwlState::TAP: {
//            LogWrapper::debug("STATE TAP");
            mTargetTapTimeCollector = 0.f;
            auto newVelocity = mPtrBirdPhysics->GetVelocity();
            CalculateTapVelocity(newVelocity);
            mPtrBirdPhysics->SetVelocity(newVelocity);
            mCurrentState = OwlState::DEFAULT;
            break;
        }

//        case OwlState::TAP_CHECK_TARGET_DISTANCE: {
//            LogWrapper::debug("STATE TAP_CHECK_TARGET_DISTANCE");
//            mTargetTapTimeCollector += dt;
//            if (mTargetTapTime - mTargetTapTimeCollector < 0) {
//                mCurrentState = OwlState::DEFAULT;
//                mPtrBirdPhysics->SetVelocityDefault();
//            }
//            break;
//        }
    }

//    if(mPtrBirdPhysics->GetVelocity().y < 0.f) LogWrapper::debug("Pos = %f", mPtrBirdPhysics->GetPosition().y);
//    if(mPtrBirdPhysics->GetVelocity().y < 0.f) LogWrapper::debug("Pos = ")
    mPtrBird->Update(deltaSec);
}

void Scene::Draw() {
    mPtrSpriteRenderer->Draw(mPtrBird);
}

void Scene::CalculateTapVelocity(glm::vec2 & velocity) {
    velocity.y = -sqrtf(2.f* mPtrBirdPhysics->GetAcceleration().y * mTargetTapDistance);
    LogWrapper::debug("Time %f", velocity.y / mPtrBirdPhysics->GetAcceleration().y);
}

