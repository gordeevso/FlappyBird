#pragma once

#include <random>

#include "Actor.h"
#include "ActorFactory.h"
#include "SpriteRenderer.h"

class Scene {

public:
    Scene();
    ~Scene() = default;
    void Update(double deltaSec);
    void Draw();

    void InputTap(bool isTapped) { mCheckInputTap = isTapped; }

private:
    void CalculateTapVelocity(glm::vec2 & velocity);
    void CalculateColumnPos();
    bool IsSeen();
private:
    enum class OwlState {
        FALL,
        TAP,
    };

    enum class ColumnState {
        WAIT,
        CALCULATE,
        SHOW
    };

private:
    std::shared_ptr<Actors::Actor> mPtrBird;
    std::shared_ptr<Actors::Actor> mPtrTopTree;
    std::shared_ptr<Actors::Actor> mPtrBottomTree;

    std::shared_ptr<Actors::PhysicsComponent> mPtrPBird;
    std::shared_ptr<Actors::PhysicsComponent> mPtrPTop;
    std::shared_ptr<Actors::PhysicsComponent> mPtrPBottom;

    std::unique_ptr<Actors::ActorFactory> mPtrActorFactory;

    std::unique_ptr<SpriteRenderer> mPtrSpriteRenderer;


    float mTargetTapDistance;
    float mTargetTapTime;
    bool mCheckInputTap;
    float mTargetColumnTopDownDistance;
    float mTargetColumnLeftRightDistance;
    float mTargetColumnMinBorderDistance;

    OwlState mBirdState;
    ColumnState mColumnState;

    std::minstd_rand0 mRandGenerator;
};


