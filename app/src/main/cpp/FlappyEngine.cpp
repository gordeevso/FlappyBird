#include "FlappyEngine.h"
#include "ResourceManager.h"
#include "Android.h"
#include "Log.h"
#include "ActorFactory.h"

#include "StringUtils.h"

FlappyEngine::FlappyEngine() : mPtrTimeManager {new TimeManager},
                               mPtrGameScene {nullptr},
                               mPtrPauseScene {nullptr},
                               mInitializedResource {false},
                               mGameState {GameState::START},
                               mTimeAccumulator {0.f},
                               mCurrentFPS {0.f}
{}

void FlappyEngine::Init() {

    if(!mInitializedResource) {
        ResourceManager::LoadTexture("textures/bird1.png", GL_TRUE, "bird1");
        ResourceManager::LoadTexture("textures/bird2.png", GL_TRUE, "bird2");
        ResourceManager::LoadTexture("textures/bird3.png", GL_TRUE, "bird3");
        ResourceManager::LoadTexture("textures/bird4.png", GL_TRUE, "bird4");
        ResourceManager::LoadTexture("textures/column.png", GL_TRUE, "column");

        mPtrGameScene.reset(new SceneGame);
        mPtrPauseScene.reset(new ScenePause{"TAP TO CONTINUE"});
        mPtrStartScene.reset(new ScenePause{"TAP TO START"});
        mPtrFinishScene.reset(new ScenePause{"TAP TO TRY AGAIN"});

        mPtrTextDefaultRenderer.reset(new TextRenderer{"fonts/slkscr.ttf", 40});
        mPtrTextPauseRenderer.reset(new TextRenderer{"fonts/luckiestguy.ttf", 40});
        mPtrTextDigitRenderer.reset(new TextRenderer{"fonts/slkscr.ttf", 40});

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


    switch (mGameState) {
        case GameState::START : {
            mPtrStartScene->Draw(mPtrTextPauseRenderer);

            if(Android::GetInstance().UpdateInput()) {
                mGameState = GameState::ACTIVE;
            }

            break;
        }

        case GameState::ACTIVE: {
            glClearColor(0.f, 0.4f, 0.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            mPtrGameScene->InputTap(Android::GetInstance().UpdateInput());
            if (!mPtrGameScene->Update(mPtrTimeManager->FrameTime()))
                mGameState = GameState::FINISH;
            mPtrGameScene->Draw();

            DrawFPSWithTargetFrequency(static_cast<float>(mPtrTimeManager->FrameTime()), 0.2f);

            break;
        }

        case GameState::PAUSE: {
            if(Android::GetInstance().UpdateInput()) {
                mPtrGameScene->RestartGame();
                mGameState = GameState::ACTIVE;
            }

            mPtrPauseScene->Draw(mPtrTextPauseRenderer);

            break;
        }

        case GameState::FINISH : {
            if(Android::GetInstance().UpdateInput()) {
                mPtrGameScene->RestartGame();
                mGameState = GameState::ACTIVE;
            }

            mPtrFinishScene->Draw(mPtrTextPauseRenderer);

            break;
        }
    }


    GLState::GetInstance().Swap();
    return true;
}

void FlappyEngine::DrawFPSWithTargetFrequency(float deltaSec, float secBetweenUpdate) {
    mTimeAccumulator += deltaSec;
    if(mTimeAccumulator >= secBetweenUpdate ) {
        mCurrentFPS = static_cast<int32_t>(mPtrTimeManager->FramesPerSecond());
        mTimeAccumulator = 0.f;
    }

    DrawFPS(to_string(mCurrentFPS));
}

void FlappyEngine::DrawFPS(std::string fps) {
    mPtrTextDefaultRenderer->RenderText(fps,
                                 GLState::GetInstance().GetScreenWidth() - mPtrTextDefaultRenderer->GetCharMap().find('a')->second.mSize.x * fps.size() - 20.f,
                                 GLState::GetInstance().GetScreenHeight() - mPtrTextDefaultRenderer->GetCharMap().find('a')->second.mSize.y - 20.f,
                                 1.0f,
                                 glm::vec3(0.2f, 0.2f, 0.8f));
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







