#include "FlappyEngine.h"
#include "Android.h"
#include "Log.h"
#include "ActorFactory.h"
#include "EventManager.h"
#include "Events.h"
#include "Utilities.h"
#include "ResourceManager.h"

GameState FlappyEngine::sGameState = GameState::START;

FlappyEngine::FlappyEngine() : mPtrGameScene {nullptr},
                               mPtrPauseScene {nullptr},
                               mInitializedResource {false},
                               mTimeAccumulator {0.f},
                               mCurrentFPS {0.f}
{
    Events::EventListenerDelegate delegate;
    delegate = std::bind(&FlappyEngine::LoadResourcesDelegate, this, std::placeholders::_1);;
    Events::EventManager::Get().AddListener(delegate, Events::EventLoadResources::sEventType);
    delegate = std::bind(&FlappyEngine::UnloadResourcesDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().AddListener(delegate, Events::EventUnloadResources::sEventType);
    delegate = std::bind(&FlappyEngine::ChangeGameStateDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().AddListener(delegate, Events::EventChangeGameState::sEventType);
}

void FlappyEngine::LoadResources() {

    if(!mInitializedResource) {
        ResourceManager::LoadTexture("textures/bird1.png", GL_TRUE, "bird1");
        ResourceManager::LoadTexture("textures/bird2.png", GL_TRUE, "bird2");
        ResourceManager::LoadTexture("textures/bird3.png", GL_TRUE, "bird3");
        ResourceManager::LoadTexture("textures/bird4.png", GL_TRUE, "bird4");
        ResourceManager::LoadTexture("textures/column.png", GL_TRUE, "column");

        ResourceManager::LoadUiStrings("xmlSettings/ui.xml");

        mPtrGameScene.reset(new SceneGame);
        mPtrPauseScene.reset(new ScenePause{"TAP TO CONTINUE"});
        mPtrStartScene.reset(new ScenePause{"TAP TO START"});
        mPtrFinishScene.reset(new ScenePause{"TAP TO TRY AGAIN"});

        mPtrUi.reset(new Ui);
        mPtrUi->LoadResources("xmlSettings/ui.xml");

        mInitializedResource = true;
    }
}


void FlappyEngine::UnloadResources() {

    if(mInitializedResource) {
        ResourceManager::Free();

        mPtrGameScene.reset(nullptr);
        mPtrPauseScene.reset(nullptr);
        mPtrStartScene.reset(nullptr);
        mPtrFinishScene.reset(nullptr);

        mPtrUi.reset(nullptr);

        mInitializedResource = false;
    }
}

FlappyEngine::~FlappyEngine() {
    Events::EventListenerDelegate delegate;
    delegate = std::bind(&FlappyEngine::LoadResourcesDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().RemoveListener(delegate, Events::EventLoadResources::sEventType);
    delegate = std::bind(&FlappyEngine::UnloadResourcesDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().RemoveListener(delegate, Events::EventUnloadResources::sEventType);
    delegate = std::bind(&FlappyEngine::ChangeGameStateDelegate, this, std::placeholders::_1);
    Events::EventManager::Get().RemoveListener(delegate, Events::EventChangeGameState::sEventType);
}

void FlappyEngine::Run() {
    TimeManager::GetInstance().UpdateMainLoop();
    Android::GetInstance().Run();
}


bool FlappyEngine::onActivate() {
    return true;
}

void FlappyEngine::onDeactivate() {
    Log::debug("FlappyEngine::onDeactivate()");
}

bool FlappyEngine::onStep() {
    TimeManager::GetInstance().UpdateMainLoop();

    switch (sGameState) {
        case GameState::START : {
            if(Android::GetInstance().UpdateInput()) {
                sGameState = GameState::ACTIVE;
            }
            break;
        }

        case GameState::ACTIVE: {
            glClearColor(0.f, 0.4f, 0.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            mPtrGameScene->InputTap(Android::GetInstance().UpdateInput());
            mPtrGameScene->Update(TimeManager::GetInstance().FrameTime());
            mPtrGameScene->Draw();

//            DrawFPSWithTargetFrequency(static_cast<float>(TimeManager::GetInstance().FrameTime()), 0.2f);
            break;
        }

        case GameState::PAUSE: {
            if(Android::GetInstance().UpdateInput()) {
                sGameState = GameState::ACTIVE;
            }
            break;
        }

        case GameState::FINISH : {
            if(Android::GetInstance().UpdateInput()) {
                mPtrGameScene->RestartGame();
                sGameState = GameState::ACTIVE;
            }
            break;
        }
    }

    mPtrUi->Draw();

    GLState::GetInstance().Swap();
    return true;
}

void FlappyEngine::DrawFPSWithTargetFrequency(float deltaSec, float secBetweenUpdate) {
    mTimeAccumulator += deltaSec;
    if(mTimeAccumulator >= secBetweenUpdate ) {
        mCurrentFPS = static_cast<int32_t>(TimeManager::GetInstance().FramesPerSecond());
        mTimeAccumulator = 0.f;
    }

    DrawFPS(to_string(mCurrentFPS));
}

void FlappyEngine::DrawFPS(std::string fps) {
//    mPtrTextDefaultRenderer->RenderText(fps,
//                                        {20.f,20.f},
//                                        {40.f,40.f},
//                                 glm::vec3(0.2f, 0.2f, 0.8f));
}

void FlappyEngine::onStart() {
}
void FlappyEngine::onResume() {
}
void FlappyEngine::onPause() {
}
void FlappyEngine::onStop() {
}
void FlappyEngine::onDestroy() {
}

void FlappyEngine::onSaveInstanceState(void **pData, size_t *pSize) {

}

void FlappyEngine::onConfigurationChanged() {
}
void FlappyEngine::onLowMemory() {
}
void FlappyEngine::onCreateWindow() {
}
void FlappyEngine::onDestroyWindow() {
    Log::debug("FlappyEngine::onDestroyWindow()");
}
void FlappyEngine::onGainFocus() {
}
void FlappyEngine::onLostFocus() {
}



void FlappyEngine::LoadResourcesDelegate(Events::IEventDataPtr ptrEvent) {
    LoadResources();
}

void FlappyEngine::UnloadResourcesDelegate(Events::IEventDataPtr ptrEvent) {
    UnloadResources();
}

void FlappyEngine::ChangeGameStateDelegate(Events::IEventDataPtr ptrEvent) {
    std::shared_ptr<Events::EventChangeGameState> ptrCastedEvent = std::static_pointer_cast<Events::EventChangeGameState>(ptrEvent);
    sGameState = ptrCastedEvent->GetNextGameState();
}










