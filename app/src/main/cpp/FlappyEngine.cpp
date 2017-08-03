#include "FlappyEngine.h"
#include "ResourceManager.h"
#include "Android.h"
#include "Log.h"
#include "ActorFactory.h"

FlappyEngine::FlappyEngine() : mPtrTimeManager {new TimeManager},
                               mPtrGameScene {nullptr},
                               mPtrPauseScene {nullptr},
                               mInitializedResource {false},
                               mGameState {GameState::PAUSE}
{}

void FlappyEngine::Init() {

    if(!mInitializedResource) {
        ResourceManager::LoadTexture("textures/bird1.png", GL_TRUE, "bird1");
        ResourceManager::LoadTexture("textures/bird2.png", GL_TRUE, "bird2");
        ResourceManager::LoadTexture("textures/bird3.png", GL_TRUE, "bird3");
        ResourceManager::LoadTexture("textures/bird4.png", GL_TRUE, "bird4");
        ResourceManager::LoadTexture("textures/column.png", GL_TRUE, "column");

        mPtrGameScene.reset(new Scene);
        mPtrPauseScene.reset(new PauseScene);

        mInitializedResource = true;
    }

}


void FlappyEngine::Run() {
    mPtrTimeManager->UpdateMainLoop();
    Android::GetInstance().Run();
}


bool FlappyEngine::onActivate() {
    return true;
}

void FlappyEngine::onDeactivate() {
    Log::debug("FlappyEngine::onDeactivate()");
}

bool FlappyEngine::onStep() {
    mPtrTimeManager->UpdateMainLoop();

//    Log::debug("FPS = %f", mPtrTimeManager->FramesPerSecond());

    glClearColor(0.f, 0.4f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (mGameState) {
        case GameState::ACTIVE: {
            mPtrGameScene->InputTap(Android::GetInstance().UpdateInput());
            if (!mPtrGameScene->Update(mPtrTimeManager->FrameTime()))
                mGameState = GameState::PAUSE;
            mPtrGameScene->Draw();
            break;
        }

        case GameState::PAUSE: {
            if(Android::GetInstance().UpdateInput())
                mGameState = GameState::ACTIVE;
            break;
        }
    }


    GLState::GetInstance().Swap();
    return true;
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
//    mPtrGLContext->Invalidate();
}

void FlappyEngine::onGainFocus() {

}

void FlappyEngine::onLostFocus() {

}





