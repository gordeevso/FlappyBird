#pragma once

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

private:
    enum class OwlState {
        DEFAULT,
        TAP,
        TAP_CHECK_TARGET_DISTANCE
    };

private:
    std::shared_ptr<Actors::Actor> mPtrBird;
    std::unique_ptr<Actors::ActorFactory> mPtrActorFactory;
    std::unique_ptr<SpriteRenderer> mPtrSpriteRenderer;
    std::shared_ptr<Actors::PhysicsComponent> mPtrBirdPhysics;
    float mTargetTapDistance;
    float mTargetTapTime;
    float mTargetTapTimeCollector;
    bool mCheckInputTap;

    OwlState mCurrentState;
};


