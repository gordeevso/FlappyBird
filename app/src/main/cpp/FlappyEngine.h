#pragma once

#include <memory>

#include "TimeManager.h"
#include "SpriteRenderer.h"
#include "ActorFactory.h"
#include "Scene.h"

class FlappyEngine
{

public:
    static FlappyEngine & GetInstance() {
        static FlappyEngine instance;
        return instance;
    }

private:
    FlappyEngine();
    FlappyEngine(FlappyEngine const &) = delete;
    FlappyEngine & operator=(FlappyEngine const &) = delete;

public:

    ~FlappyEngine() = default;
    void Run();
    void Init();

    bool onActivate();
    void onDeactivate();
    bool onStep();

    void onStart();
    void onResume();
    void onPause();
    void onStop();
    void onDestroy();

    void onSaveInstanceState(void** pData, size_t* pSize);
    void onConfigurationChanged();
    void onLowMemory();

    void onCreateWindow();
    void onDestroyWindow();
    void onGainFocus();
    void onLostFocus();

private:

    enum class GameState {
        ACTIVE,
        PAUSE
    };

private:

    std::unique_ptr<TimeManager> mPtrTimeManager;
    std::unique_ptr<Scene> mPtrGameScene;
    std::unique_ptr<PauseScene> mPtrPauseScene;
    bool mInitializedResource;
    GameState mGameState;
};
