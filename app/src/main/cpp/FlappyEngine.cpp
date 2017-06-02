#include "FlappyEngine.h"

FlappyEngine::FlappyEngine() : mPtrAndroidApp(nullptr),
                               mPtrGLContext(nullptr),
                               mInitializedResource(false),
                               mHasFocus(false)
{}

void FlappyEngine::Init(android_app *app) {
    mPtrAndroidApp = app;

    mPtrAndroidApp->userData = this;
    mPtrAndroidApp->onAppCmd = AndroidStateHandler;
    mPtrAndroidApp->onInputEvent = AndroidTouchHandler;

    mPtrGLContext.reset(new GLContextWrapper);
    TapDetector::GetInstance()->SetConfiguration(mPtrAndroidApp->config);
}


FlappyEngine::~FlappyEngine() {

}


void FlappyEngine::Run() {
    app_dummy();
    LogWrapper::debug("FlappyEngine::Run() started");



    while(true) {
        int id;
        int events;
        android_poll_source* source;

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
                onDestroyWindow();
                return;
            }
        }

        if (mHasFocus && !mFinishActivity) {

            if (!onStep()) {
                mFinishActivity = true;
                LogWrapper::debug("Finishing activity from main loop");
                ANativeActivity_finish(mPtrAndroidApp->activity);
            }
        }
    }
}


void FlappyEngine::GetFocus() {
    LogWrapper::debug("FlappyEngine::GetFocus()");
    if (!mHasFocus && mPtrAndroidApp->window) {
        mHasFocus = true;
        mFinishActivity = false;

        if (!onActivate()) {
            mFinishActivity = true;
            GiveFocus();
            ANativeActivity_finish(mPtrAndroidApp->activity);
        }
    }
}

void FlappyEngine::GiveFocus() {
    LogWrapper::debug("FlappyEngine::GiveFocus()");
    if (mHasFocus) {
        onDeactivate();
        mHasFocus = false;
    }
}


void FlappyEngine::AndroidStateHandler(struct android_app *app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_CONFIG_CHANGED:
            LogWrapper::debug("APP_CMD_CONFIG_CHANGED");
            FlappyEngine::GetInstance()->onConfigurationChanged();
            break;
        case APP_CMD_INIT_WINDOW:
            LogWrapper::debug("APP_CMD_INIT_WINDOW");
            FlappyEngine::GetInstance()->onCreateWindow();
            break;
        case APP_CMD_DESTROY:
            LogWrapper::debug("APP_CMD_DESTROY");
            FlappyEngine::GetInstance()->onDestroy();
            break;
        case APP_CMD_GAINED_FOCUS:
            LogWrapper::debug("APP_CMD_GAINED_FOCUS");
            FlappyEngine::GetInstance()->GetFocus();
            FlappyEngine::GetInstance()->onGainFocus();
            break;
        case APP_CMD_LOST_FOCUS:
            LogWrapper::debug("APP_CMD_LOST_FOCUS");
            FlappyEngine::GetInstance()->onLostFocus();
            FlappyEngine::GetInstance()->GiveFocus();
            break;
        case APP_CMD_LOW_MEMORY:
            LogWrapper::debug("APP_CMD_LOW_MEMORY");
            FlappyEngine::GetInstance()->onLowMemory();
            break;
        case APP_CMD_PAUSE:
            LogWrapper::debug("APP_CMD_PAUSE");
            FlappyEngine::GetInstance()->onPause();
            FlappyEngine::GetInstance()->GiveFocus();
            break;
        case APP_CMD_RESUME:
            LogWrapper::debug("APP_CMD_RESUME");
            FlappyEngine::GetInstance()->onResume();
            break;
        case APP_CMD_SAVE_STATE:
            LogWrapper::debug("APP_CMD_SAVE_STATE");
            FlappyEngine::GetInstance()->onSaveInstanceState(
                    &FlappyEngine::GetInstance()->mPtrAndroidApp->savedState,
                    &FlappyEngine::GetInstance()->mPtrAndroidApp->savedStateSize);
            break;
        case APP_CMD_START:
            LogWrapper::debug("APP_CMD_START");
            FlappyEngine::GetInstance()->onStart();
            break;
        case APP_CMD_STOP:
            LogWrapper::debug("APP_CMD_STOP");
            FlappyEngine::GetInstance()->onStop();
            break;
        case APP_CMD_TERM_WINDOW:
            LogWrapper::debug("APP_CMD_TERM_WINDOW");
            FlappyEngine::GetInstance()->onDestroyWindow();
            FlappyEngine::GetInstance()->GiveFocus();
            break;
        default:
            break;
    }
}

int32_t FlappyEngine::AndroidTouchHandler(android_app *app, AInputEvent *event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        TouchState tapState = TapDetector::GetInstance()->Detect(event);

        if(tapState == GESTURE_STATE_ACTION) {
            LogWrapper::debug("TAP");
        }
        return 1;
    }
    return 0;
}


void FlappyEngine::DrawFrame() {

}


void FlappyEngine::TrimMemory() {

}

bool FlappyEngine::onActivate() {
    mPtrGLContext->Resume(mPtrAndroidApp->window);
    return true;
}

void FlappyEngine::onDeactivate() {
    LogWrapper::debug("FlappyEngine::onDeactivate()");
    mPtrGLContext->Suspend();
}

bool FlappyEngine::onStep() {
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
    mPtrGLContext->Invalidate();
}

void FlappyEngine::onGainFocus() {

}

void FlappyEngine::onLostFocus() {

}




