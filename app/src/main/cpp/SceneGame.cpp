#include <cmath>
#include <chrono>
#include <numeric>

#include "SceneGame.h"
#include "GLState.h"
#include "Android.h"
#include "EventManager.h"
#include "Events.h"
#include "GameTypes.h"

SceneGame::SceneGame() : mPtrBird {nullptr},
                 mPtrPBird {nullptr},
                 mVecBarriers {},
                 mPtrActorFactory {new Actors::ActorFactory},
                 mPtrSpriteRenderer {new SpriteRenderer},
                 mTargetTapDistance {},
                 mTargetTapTime {},
                 mCheckInputTap {false},
                 mCurrentScore {0},
                 mBirdState {OwlState::TAP},
                 mRandGenerator {static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count())}
{
    tinyxml2::XMLDocument sceneXml;
    std::vector<uint8_t> xmlBuffer;
    ResourceManager::Read("xmlSettings/scene.xml", xmlBuffer);
    auto result = sceneXml.Parse(std::string(xmlBuffer.begin(), xmlBuffer.end()).c_str());

    Log::debug("LOAD Settings xmlSettings/scene.xml");
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        assert(result == tinyxml2::XMLError::XML_SUCCESS);
    }

    tinyxml2::XMLElement *sceneXmlRoot = sceneXml.RootElement();
    mTargetTapDistance = sceneXmlRoot->FloatAttribute("TargetTapDistance");
    mTargetTapTime = sceneXmlRoot->FloatAttribute("TargetTapTime");
    mTargetColumnTopDownDistance = sceneXmlRoot->FloatAttribute("TargetColumnTopDownDistance");
    mTargetColumnLeftRightDistance = sceneXmlRoot->FloatAttribute("TargetColumnLeftRightDistance");
    mTargetColumnMinBorderDistance = sceneXmlRoot->FloatAttribute("TargetColumnMinBorderDistance");

    assert(static_cast<float>(GLState::GetInstance().GetScreenHeight()) -
           2.f * mTargetColumnMinBorderDistance -
           mTargetColumnTopDownDistance > 0.f);

    mPtrBird = mPtrActorFactory->CreateActor("xmlSettings/owl.xml");
    auto ptrWeakPhysicsComponentBird = mPtrBird->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
    mPtrPBird = Actors::MakeStrongPtr(ptrWeakPhysicsComponentBird);

    int32_t barrierCount = CalculateBarriersCount();
    assert(barrierCount > 0);
    mVecBarriers.resize(static_cast<size_t>(barrierCount));

    for(auto & barrier : mVecBarriers) {
        barrier.mPtrATopColumn = mPtrActorFactory->CreateActor("xmlSettings/topColumn.xml");
        auto ptrWeakPhysicsComponentTop = barrier.mPtrATopColumn->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
        barrier.mPtrPTopColumn = Actors::MakeStrongPtr(ptrWeakPhysicsComponentTop);

        barrier.mPtrABottomColumn = mPtrActorFactory->CreateActor("xmlSettings/bottomColumn.xml");
        auto ptrWeakPhysicsComponentBottom = barrier.mPtrABottomColumn->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
        barrier.mPtrPBottomColumn = Actors::MakeStrongPtr(ptrWeakPhysicsComponentBottom);

        barrier.mBarrierState = BarrierState::CALCULATE;
    }

}


void SceneGame::RestartGame() {
    for(auto & barrier : mVecBarriers) {
        barrier.mBarrierState = BarrierState::CALCULATE;
    }

    mPtrBird->Destroy();
    mPtrBird.reset();

    mPtrBird = mPtrActorFactory->CreateActor("xmlSettings/owl.xml");
    auto ptrWeakPhysicsComponentBird = mPtrBird->GetComponent<Actors::PhysicsComponent>("PhysicsComponent");
    mPtrPBird = Actors::MakeStrongPtr(ptrWeakPhysicsComponentBird);

}


void SceneGame::Update(double deltaSec) {

    switch(mBirdState) {
        case OwlState::FALL: {
            if(mCheckInputTap) mBirdState = OwlState::TAP;
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

    for(auto & barrier: mVecBarriers) {
        switch (barrier.mBarrierState) {
            case BarrierState::WAIT : {
                if(ShowBarrier(barrier.mPtrPTopColumn->GetPosition().x)) barrier.mBarrierState = BarrierState::SHOW;
                break;
            }

            case BarrierState::CALCULATE : {
                CalculateColumnPos(barrier.mPtrPTopColumn, barrier.mPtrPBottomColumn);
                Log::debug("Barr pos %f %f", barrier.mPtrPTopColumn->GetPosition().x, barrier.mPtrPTopColumn->GetPosition().y);
                barrier.mBarrierState = BarrierState::WAIT;
                break;
            }

            case BarrierState::SHOW : {
                if(mPtrPBird->CheckCollision(*barrier.mPtrPTopColumn) ||
                   mPtrPBird->CheckCollision(*barrier.mPtrPBottomColumn)) {
                    Events::EventManager::Get().QueueEvent(std::shared_ptr<Events::EventChangeGameState>(
                            new Events::EventChangeGameState(GameState::FINISH)));
                }


                barrier.mPtrATopColumn->Update(deltaSec);
                barrier.mPtrABottomColumn->Update(deltaSec);
                if (!IsSeen(barrier.mPtrPTopColumn)) barrier.mBarrierState = BarrierState::CALCULATE;
                break;
            }
        }
    }

//    Log::debug("Bird position %f , %f", mPtrPBird->GetPosition().x, mPtrPBird->GetPosition().y);
    if(CheckBirdOverlapScene()) {
        Events::EventManager::Get().QueueEvent(std::shared_ptr<Events::EventChangeGameState>(
                new Events::EventChangeGameState(GameState::FINISH)));
    }

    mPtrBird->Update(deltaSec);
}

void SceneGame::Draw() {
    mPtrSpriteRenderer->Draw(mPtrBird);

    for(auto & barrier: mVecBarriers) {
        switch (barrier.mBarrierState) {
            case BarrierState::SHOW : {
                mPtrSpriteRenderer->Draw(barrier.mPtrATopColumn);
                mPtrSpriteRenderer->Draw(barrier.mPtrABottomColumn);
                break;
            }

            default:
                break;
        }
    }

}

void SceneGame::CalculateTapVelocity(glm::vec2 & velocity) {
    velocity.y = -sqrtf(2.f* mPtrPBird->GetAcceleration().y * mTargetTapDistance);
//    Log::debug("Time %f", velocity.y / mPtrPBird->GetAcceleration().y);
}

bool SceneGame::IsSeen(std::shared_ptr<Actors::PhysicsComponent> ptrTop) {
    return ptrTop->GetPosition().x > -ptrTop->GetSize().x;
}

int32_t SceneGame::CalculateBarriersCount() {
    return (GLState::GetInstance().GetScreenWidth() / static_cast<int32_t>(mTargetColumnLeftRightDistance)) * 2 + 3;
}

void SceneGame::CalculateColumnPos(std::shared_ptr<Actors::PhysicsComponent> ptrTop,
                               std::shared_ptr<Actors::PhysicsComponent> ptrBottom) {
    auto TSize = ptrTop->GetSize();
    auto BSize = ptrBottom->GetSize();

    glm::vec2 TPos {};
    glm::vec2 BPos {};
    auto rand = mRandGenerator();



    float posTopColBLy = rand %
                         static_cast<uint32_t>(static_cast<float>(GLState::GetInstance().GetScreenHeight()) -
                                               2.f * mTargetColumnMinBorderDistance -
                                               mTargetColumnTopDownDistance) +
                         mTargetColumnMinBorderDistance;



    TPos.x = GLState::GetInstance().GetScreenWidth();
    TPos.y = posTopColBLy - TSize.y;
    BPos.x = GLState::GetInstance().GetScreenWidth();
    BPos.y = posTopColBLy + mTargetColumnTopDownDistance;

    ptrTop->SetPosition(TPos);
    ptrBottom->SetPosition(BPos);
}

bool SceneGame::ShowBarrier(float posX) {
    float minDist = std::numeric_limits<float>::max();

    for(auto & barrier: mVecBarriers) {
        if(barrier.mBarrierState == BarrierState::SHOW) {
            minDist = std::min(minDist, posX - barrier.mPtrPTopColumn->GetPosition().x);
        }
    }

    return minDist > mTargetColumnLeftRightDistance;
}

bool SceneGame::CheckBirdOverlapScene() {
    return mPtrPBird->GetPosition().y + mPtrPBird->GetSize().y > GLState::GetInstance().GetScreenHeight();
}






ScenePause::ScenePause(const std::string &message) : mMessage {message} {
}

void ScenePause::Update(double deltaSec) {

}

void ScenePause::Draw(std::unique_ptr<TextRenderer> const & ptrTextRenderer) {

    ptrTextRenderer->DrawStrings();
}


