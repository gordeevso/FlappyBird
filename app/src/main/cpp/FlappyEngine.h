#pragma once

#include <memory>

#include "TimeManager.h"
#include "SpriteRenderer.h"
#include "SceneGame.h"
#include "TextRenderer.h"
#include "EventManager.h"
#include "GameTypes.h"
#include "Ui.h"

class FlappyEngine
{

public:
    static FlappyEngine & GetInstance() {
        static FlappyEngine instance;
        return instance;
    }

    static GameState GetGameState() {
        return sGameState;
    }

private:
    FlappyEngine();
    FlappyEngine(FlappyEngine const &) = delete;
    FlappyEngine & operator=(FlappyEngine const &) = delete;

public:

    ~FlappyEngine();
    void Run();
    void LoadResources();
    void UnloadResources();

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

    void ChangeGameStateDelegate(Events::IEventDataPtr ptrEvent);
    void LoadResourcesDelegate(Events::IEventDataPtr ptrEvent);
    void UnloadResourcesDelegate(Events::IEventDataPtr ptrEvent);

private:

    std::unique_ptr<SceneGame> mPtrGameScene;
    std::unique_ptr<ScenePause> mPtrPauseScene;
    std::unique_ptr<ScenePause> mPtrStartScene;
    std::unique_ptr<ScenePause> mPtrFinishScene;

    std::unique_ptr<Ui> mPtrUi;
//    std::unique_ptr<TextRenderer> mPtrTextPauseRenderer;
//    std::unique_ptr<TextRenderer> mPtrTextDefaultRenderer;
//    std::unique_ptr<TextRenderer> mPtrTextDigitRenderer;

    bool mInitializedResource;
    float mTimeAccumulator;
    float mCurrentFPS;

    static GameState sGameState;

};
