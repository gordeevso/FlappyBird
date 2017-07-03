#include <cmath>
#include <chrono>

#include "Scene.h"
#include "GLContextWrapper.h"

Scene::Scene() : mPtrBird {nullptr},
                 mPtrTopTree {nullptr},
                 mPtrBottomTree {nullptr},
                 mPtrPBird {nullptr},
                 mPtrPTop {nullptr},
                 mPtrPBottom {nullptr},
                 mPtrActorFactory {new Actors::ActorFactory},
                 mPtrSpriteRenderer {new SpriteRenderer},
                 mTargetTapDistance {},
                 mTargetTapTime {},
                 mCheckInputTap {false},
                 mBirdState {OwlState::TAP},
                 mColumnState {ColumnState::CALCULATE},
                 mRandGenerator {static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count())}
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
    mTargetColumnTopDownDistance = sceneXmlRoot->FloatAttribute("TargetColumnTopDownDistance");
    mTargetColumnLeftRightDistance = sceneXmlRoot->FloatAttribute("TargetColumnLeftRightDistance");
    mTargetColumnMinBorderDistance = sceneXmlRoot->FloatAttribute("TargetColumnMinBorderDistance");

    mPtrBird = mPtrActorFactory->CreateActor("xmlSettings/owl.xml");
    auto ptrWeakPhysicsComponentBird = mPtrBird->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
    mPtrPBird = Actors::MakeStrongPtr(ptrWeakPhysicsComponentBird);

    mPtrTopTree = mPtrActorFactory->CreateActor("xmlSettings/topColumn.xml");
    mPtrBottomTree = mPtrActorFactory->CreateActor("xmlSettings/bottomColumn.xml");
    auto ptrWeakPhysicsComponentTop = mPtrTopTree->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
    mPtrPTop = Actors::MakeStrongPtr(ptrWeakPhysicsComponentTop);
    auto ptrWeakPhysicsComponentBottom = mPtrBottomTree->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
    mPtrPBottom = Actors::MakeStrongPtr(ptrWeakPhysicsComponentBottom);
}


void Scene::Update(double deltaSec) {
    auto dt = static_cast<float>(deltaSec);

    if(mCheckInputTap) mBirdState = OwlState::TAP;

    switch(mBirdState) {
        case OwlState::FALL: {
            break;
        }

        case OwlState::TAP: {
            auto newVelocity = mPtrPBird->GetVelocity();
            CalculateTapVelocity(newVelocity);
            mPtrPBird->SetVelocity(newVelocity);
            mBirdState = OwlState::FALL;
            break;
        }
    }

    switch (mColumnState) {
        case ColumnState::WAIT : {
            LogWrapper::debug("WAIT");
            mColumnState = ColumnState::SHOW;
            break;
        }

        case ColumnState::CALCULATE : {
            LogWrapper::debug("CALCULATE");
            CalculateColumnPos();
            mColumnState = ColumnState::WAIT;
            break;
        }

        case ColumnState::SHOW : {
            LogWrapper::debug("SHOW");
            mPtrTopTree->Update(deltaSec);
            mPtrBottomTree->Update(deltaSec);
            if(!IsSeen()) mColumnState = ColumnState::CALCULATE;
            break;
        }
    }
//    LogWrapper::debug("Bird position %f , %f", mPtrPBird->GetPosition().x, mPtrPBird->GetPosition().y);
    mPtrBird->Update(deltaSec);

}

void Scene::Draw() {
    mPtrSpriteRenderer->Draw(mPtrBird);

    switch (mColumnState) {
        case ColumnState::SHOW : {
            mPtrSpriteRenderer->Draw(mPtrTopTree);
            mPtrSpriteRenderer->Draw(mPtrBottomTree);
            break;
        }

        default:
            break;
    }

}

void Scene::CalculateTapVelocity(glm::vec2 & velocity) {
    velocity.y = -sqrtf(2.f* mPtrPBird->GetAcceleration().y * mTargetTapDistance);
    LogWrapper::debug("Time %f", velocity.y / mPtrPBird->GetAcceleration().y);
}

void Scene::CalculateColumnPos() {
    auto TSize = mPtrPTop->GetSize();
    auto BSize = mPtrPBottom->GetSize();

    glm::vec2 TPos {};
    glm::vec2 BPos {};

    float posTopColBLy = mRandGenerator() %
                         static_cast<uint32_t>(GLContextWrapper::GetInstance().GetScreenHeight() -
                                               2.f * mTargetColumnMinBorderDistance -
                                               mTargetColumnTopDownDistance) +
                                               mTargetColumnMinBorderDistance;

    TPos.x = GLContextWrapper::GetInstance().GetScreenWidth();
    TPos.y = posTopColBLy - TSize.y;
    BPos.x = GLContextWrapper::GetInstance().GetScreenWidth();
    BPos.y = posTopColBLy + mTargetColumnTopDownDistance;

    mPtrPTop->SetPosition(TPos);
    mPtrPBottom->SetPosition(BPos);
}

bool Scene::IsSeen() {
    return mPtrPTop->GetPosition().x > -mPtrPTop->GetSize().x;
}

