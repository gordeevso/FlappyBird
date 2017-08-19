#pragma once

#include <random>

#include "Actor.h"
#include "ActorFactory.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
//#include "Events.h"

class SceneGame {

public:
    SceneGame();
    ~SceneGame() = default;
    void Update(double deltaSec);
    void Draw();

    void InputTap(bool isTapped) { mCheckInputTap = isTapped; }
    void RestartGame();

    uint64_t GetCurrentScore() { return mCurrentScore; }

private:
    void CalculateTapVelocity(glm::vec2 & velocity);
    void CalculateColumnPos(std::shared_ptr<Actors::PhysicsComponent> ptrTop,
                            std::shared_ptr<Actors::PhysicsComponent> ptrBottom);
    int32_t CalculateBarriersCount();
    bool ShowBarrier(float posX);
    bool IsSeen(std::shared_ptr<Actors::PhysicsComponent> ptrTop);
    bool CheckBirdOverlapScene();
    bool CheckScore();
private:
    enum class OwlState {
        FALL,
        TAP,
    };

    enum class BarrierState {
        WAIT,
        CALCULATE,
        SHOW
    };

    struct Barrier {
        Barrier() : mPtrATopColumn {nullptr},
                    mPtrPTopColumn {nullptr},
                    mPtrABottomColumn {nullptr},
                    mPtrPBottomColumn {nullptr},
                    mBarrierState {BarrierState::CALCULATE}
        {}

        std::shared_ptr<Actors::Actor> mPtrATopColumn;
        std::shared_ptr<Actors::PhysicsComponent> mPtrPTopColumn;
        std::shared_ptr<Actors::Actor> mPtrABottomColumn;
        std::shared_ptr<Actors::PhysicsComponent> mPtrPBottomColumn;
        BarrierState mBarrierState;
    };

private:
    std::shared_ptr<Actors::Actor> mPtrBird;
    std::shared_ptr<Actors::PhysicsComponent> mPtrPBird;
    std::vector<Barrier> mVecBarriers;

    std::unique_ptr<Actors::ActorFactory> mPtrActorFactory;

    std::unique_ptr<SpriteRenderer> mPtrSpriteRenderer;

    float mTargetTapDistance;
    float mTargetTapTime;
    bool mCheckInputTap;
    float mTargetColumnTopDownDistance;
    float mTargetColumnLeftRightDistance;
    float mTargetColumnMinBorderDistance;
    uint64_t mCurrentScore;
    uint64_t mFinalScore;
    float mPrevMinDistBirdCol;
    OwlState mBirdState;
    std::minstd_rand0 mRandGenerator;
};


class ScenePause {

public:
    ScenePause(std::string const & message = "");
    ~ScenePause() = default;
    void Update(double deltaSec);

    void InputTap(bool isTapped) { mCheckInputTap = isTapped; }

private:
    bool mCheckInputTap;
    std::string mMessage;
};