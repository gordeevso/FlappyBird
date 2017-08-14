
#include <cassert>
#include <malloc.h>
#include <memory>

#include "Android.h"
#include "FlappyEngine.h"
#include "Log.h"
#include "Events.h"

Android::Android() : mPtrAndroidApp {nullptr},
                     mAssetManager {nullptr},
                     mAsset {nullptr},
                     mPtrTapDetector {nullptr},
                     mRequestToUnloadResources {false},
                     mTapped {false},
                     mHasFocus {false},
                     mFinishActivity {false}
{}

void Android::Init(android_app * androidApp) {
    mPtrAndroidApp = androidApp;

    mPtrAndroidApp->userData = this;
    mPtrAndroidApp->onAppCmd = AndroidStateHandler;
    mPtrAndroidApp->onInputEvent = AndroidTouchHandler;

    mPtrTapDetector.reset(new TapDetector(mPtrAndroidApp->config));
}

void Android::Run() {
    app_dummy();
    Log::debug("FlappyEngine::Run() started");

    while(true) {
        int id;
        int events;
        android_poll_source* source;

        mTapped = false;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while((id = ALooper_pollAll(mHasFocus ? 0 : -1,
                                    NULL,
                                    &events,
                                    (void**)&source)) >= 0) {

            if(source){
                source->process(mPtrAndroidApp, source);
            }

            if (mPtrAndroidApp->destroyRequested != 0) {
                Log::debug("Finishing activity from main loop");
                FlappyEngine::GetInstance().onDestroyWindow();
                GLState::GetInstance().Invalidate();
                mFinishActivity = true;
                ANativeActivity_finish(mPtrAndroidApp->activity);
                return;
            }
        }

        if (mHasFocus && !mFinishActivity) {
            FlappyEngine::GetInstance().onStep();
        }
    }
}


void Android::GetFocus() {
    Log::debug("FlappyEngine::GetFocus()");
    if (!mHasFocus && mPtrAndroidApp->window) {
        mHasFocus = true;
        mFinishActivity = false;
    }
}

void Android::GiveFocus() {
    Log::debug("FlappyEngine::GiveFocus()");
    if (mHasFocus) {
//        GLState::GetInstance().Suspend();
        mHasFocus = false;
    }
}

android_app const *Android::GetAndroidApp() const {
    return mPtrAndroidApp;
}



void Android::AndroidStateHandler(android_app *androidApp, int32_t cmd) {
    Android * androidPtr = reinterpret_cast<Android*>(androidApp->userData);
    assert(androidPtr);
    androidPtr->HandleState(cmd);
}

int32_t Android::AndroidTouchHandler(android_app *androidApp, AInputEvent *inputEvent) {
    Android * androidPtr = reinterpret_cast<Android*>(androidApp->userData);
    assert(androidPtr);
    return androidPtr->HandleInput(inputEvent);
}



void Android::HandleState(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_CONFIG_CHANGED:
            Log::debug("APP_CMD_CONFIG_CHANGED");
            FlappyEngine::GetInstance().onConfigurationChanged();
            break;


        case APP_CMD_INIT_WINDOW:
            mRequestToUnloadResources = false;
            Log::debug("APP_CMD_INIT_WINDOW");
        case APP_CMD_WINDOW_RESIZED:
            Log::debug("APP_CMD_WINDOW_RESIZED");
            GLState::GetInstance().Resume();
            FlappyEngine::GetInstance().LoadResources();
            break;

        case APP_CMD_TERM_WINDOW:
            Log::debug("APP_CMD_TERM_WINDOW");
            FlappyEngine::GetInstance().onDestroyWindow();
            if(mRequestToUnloadResources) FlappyEngine::GetInstance().UnloadResources();
            GLState::GetInstance().Suspend();
            break;

        case APP_CMD_DESTROY:
            Log::debug("APP_CMD_DESTROY");
            FlappyEngine::GetInstance().onDestroy();
            GLState::GetInstance().Invalidate();
            break;

        case APP_CMD_GAINED_FOCUS:
            Log::debug("APP_CMD_GAINED_FOCUS");
            Android::GetFocus();
            FlappyEngine::GetInstance().onGainFocus();
            break;

        case APP_CMD_PAUSE:
            Log::debug("APP_CMD_PAUSE");
        case APP_CMD_LOST_FOCUS:
            Log::debug("APP_CMD_LOST_FOCUS");
            FlappyEngine::GetInstance().onLostFocus();
            Android::GiveFocus();
            break;

        case APP_CMD_LOW_MEMORY:
            Log::debug("APP_CMD_LOW_MEMORY");
            FlappyEngine::GetInstance().onLowMemory();
            break;

        case APP_CMD_RESUME:
            Log::debug("APP_CMD_RESUME");
            FlappyEngine::GetInstance().onResume();
            break;

        case APP_CMD_SAVE_STATE:
            Log::debug("APP_CMD_SAVE_STATE");
//            mPtrAndroidApp->savedState = malloc(sizeof(struct saved_state));
//            *((struct saved_state*)mPtrAndroidApp->savedState) = mState;
//            mPtrAndroidApp->savedStateSize = sizeof(struct saved_state);
//
//            FlappyEngine::GetInstance().onSaveInstanceState(
//                    &mPtrAndroidApp->savedState,
//                    &mPtrAndroidApp->savedStateSize);
            break;

        case APP_CMD_START:
            Log::debug("APP_CMD_START");
            FlappyEngine::GetInstance().onStart();
            break;

        case APP_CMD_STOP:
            Log::debug("APP_CMD_STOP");
            FlappyEngine::GetInstance().onStop();
            break;

        default:
            break;
    }

}

int32_t Android::HandleInput(AInputEvent *inputEvent) {
    if (AKeyEvent_getKeyCode(inputEvent) == AKEYCODE_BACK) {
        mRequestToUnloadResources = true;
    }

    if (AInputEvent_getType(inputEvent) == AINPUT_EVENT_TYPE_MOTION) {
        TouchState tapState = mPtrTapDetector->Detect(inputEvent);

        if(tapState == GESTURE_STATE_ACTION) {
            Log::debug("TAP");
            mTapped = true;
        }

        return 1;
    }
    return 0;
}


