#include "FlappyEngine.h"
#include "ResourceManager.h"
#include "Android.h"
#include "LogWrapper.h"

FlappyEngine::FlappyEngine() : mPtrTimeManager {new TimeManager},
                               mPtrSpriteRenderer {nullptr},
                               mInitializedResource {false}
{}

void FlappyEngine::Init() {

    if(!mInitializedResource) {
//        ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", "rect");
        mPtrSpriteRenderer.reset(new SpriteRenderer);
        mInitializedResource = true;
    }

}


void FlappyEngine::Run() {
    mPtrTimeManager->UpdateMainLoop();


    Android::GetInstance().Run();
}


bool FlappyEngine::onActivate() {
//    mPtrGLContext->Resume(mPtrAndroidApp->window);
    return true;
}

void FlappyEngine::onDeactivate() {
    LogWrapper::debug("FlappyEngine::onDeactivate()");
//    mPtrGLContext->Suspend();
}

bool FlappyEngine::onStep() {
    mPtrTimeManager->UpdateMainLoop();

//    LogWrapper::debug("FPS = %f", mPtrTimeManager->FramesPerSecond());


    glClearColor(0.f, 0.4f, 0.f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mPtrSpriteRenderer->DrawSprite(ResourceManager::GetTexture("cat"), {0.f, 0.f}, {128.f, 128.f}, 0.f, {1.f,1.f,1.f});



    GLContextWrapper::GetInstance().Swap();
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
    LogWrapper::debug("FlappyEngine::onDestroyWindow()");
//    mPtrGLContext->Invalidate();
}

void FlappyEngine::onGainFocus() {

}

void FlappyEngine::onLostFocus() {

}





