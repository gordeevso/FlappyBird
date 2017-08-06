#pragma once

#include <memory>

#include "TimeManager.h"
#include "SpriteRenderer.h"
#include "ActorFactory.h"
#include "SceneGame.h"
#include "TextRenderer.h"

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

    void DrawFPSWithTargetFrequency(float deltaSec, float secBetweenUpdate);
    void DrawFPS(std::string fps);
private:

    enum class GameState {
        START,
        ACTIVE,
        PAUSE,
        FINISH
    };

private:

    std::unique_ptr<TimeManager> mPtrTimeManager;

    std::unique_ptr<SceneGame> mPtrGameScene;
    std::unique_ptr<ScenePause> mPtrPauseScene;
    std::unique_ptr<ScenePause> mPtrStartScene;
    std::unique_ptr<ScenePause> mPtrFinishScene;

    std::unique_ptr<TextRenderer> mPtrTextDefaultRenderer;
    std::unique_ptr<TextRenderer> mPtrTextPauseRenderer;
    std::unique_ptr<TextRenderer> mPtrTextDigitRenderer;

    bool mInitializedResource;
    GameState mGameState;
    float mTimeAccumulator;
    float mCurrentFPS;
};
