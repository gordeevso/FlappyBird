#include "FlappyEngine.h"

FlappyEngine::FlappyEngine() : mPtrAndroidApp(nullptr),
                               mPtrGLContext(nullptr),
                               mPtrSensorManager(nullptr),
                               mInitializedResource(false),
                               mHasFocus(false)
{}

void FlappyEngine::Init(android_app *app) {
    mPtrAndroidApp = app;

    mPtrAndroidApp->userData = this;
    mPtrAndroidApp->onAppCmd = AndroidStateHandler;
    mPtrAndroidApp->onInputEvent = AndroidInputHandler;

    mPtrGLContext.reset(new GLContextWrapper);
    mPtrSensorManager.reset(new SensorManager(app));
}


FlappyEngine::~FlappyEngine() {

}


void FlappyEngine::Run() {
    app_dummy();
    LogWrapper::info("FlappyEngine::Run() started");



    // loop waiting for stuff to do.
    while(true) {
        // Read all pending events.
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
            // Process this event.
            if(source){
                source->process(mPtrAndroidApp, source);
            }

            mPtrSensorManager->Process(id);

            // Check if we are exiting.
            if (mPtrAndroidApp->destroyRequested != 0) {
                onDestroyWindow();
                return;
            }
        }

        if (mHasFocus && !mFinishActivity) {
            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
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
    // Enables activity only if a window is available.
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
            LogWrapper::info("APP_CMD_CONFIG_CHANGED");
            FlappyEngine::GetInstance()->onConfigurationChanged();
            break;
        case APP_CMD_INIT_WINDOW:
            LogWrapper::info("APP_CMD_INIT_WINDOW");
            FlappyEngine::GetInstance()->onCreateWindow();
            break;
        case APP_CMD_DESTROY:
            LogWrapper::info("APP_CMD_DESTROY");
            FlappyEngine::GetInstance()->onDestroy();
            break;
        case APP_CMD_GAINED_FOCUS:
            LogWrapper::info("APP_CMD_GAINED_FOCUS");
            FlappyEngine::GetInstance()->GetFocus();
            FlappyEngine::GetInstance()->onGainFocus();
            break;
        case APP_CMD_LOST_FOCUS:
            LogWrapper::info("APP_CMD_LOST_FOCUS");
            FlappyEngine::GetInstance()->onLostFocus();
            FlappyEngine::GetInstance()->GiveFocus();
            break;
        case APP_CMD_LOW_MEMORY:
            LogWrapper::info("APP_CMD_LOW_MEMORY");
            FlappyEngine::GetInstance()->onLowMemory();
            break;
        case APP_CMD_PAUSE:
            LogWrapper::info("APP_CMD_PAUSE");
            FlappyEngine::GetInstance()->onPause();
            FlappyEngine::GetInstance()->GiveFocus();
            break;
        case APP_CMD_RESUME:
            LogWrapper::info("APP_CMD_RESUME");
            FlappyEngine::GetInstance()->onResume();
            break;
        case APP_CMD_SAVE_STATE:
            LogWrapper::info("APP_CMD_SAVE_STATE");
            FlappyEngine::GetInstance()->onSaveInstanceState(
                    &FlappyEngine::GetInstance()->mPtrAndroidApp->savedState,
                    &FlappyEngine::GetInstance()->mPtrAndroidApp->savedStateSize);
            break;
        case APP_CMD_START:
            LogWrapper::info("APP_CMD_START");
            FlappyEngine::GetInstance()->onStart();
            break;
        case APP_CMD_STOP:
            LogWrapper::info("APP_CMD_STOP");
            FlappyEngine::GetInstance()->onStop();
            break;
        case APP_CMD_TERM_WINDOW:
            LogWrapper::info("APP_CMD_TERM_WINDOW");
            FlappyEngine::GetInstance()->onDestroyWindow();
            FlappyEngine::GetInstance()->GiveFocus();
            break;
        default:
            break;
    }
}

int32_t FlappyEngine::AndroidInputHandler(android_app *app, AInputEvent *event) {
//    Engine* eng = (Engine*)app->userData;
//    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
//        ndk_helper::GESTURE_STATE doubleTapState =
//                eng->doubletap_detector_.Detect(event);
//        ndk_helper::GESTURE_STATE dragState = eng->drag_detector_.Detect(event);
//        ndk_helper::GESTURE_STATE pinchState = eng->pinch_detector_.Detect(event);
//
//        // Double tap detector has a priority over other detectors
//        if (doubleTapState == ndk_helper::GESTURE_STATE_ACTION) {
//            // Detect double tap
//            eng->tap_camera_.Reset(true);
//        } else {
//            // Handle drag state
//            if (dragState & ndk_helper::GESTURE_STATE_START) {
//                // Otherwise, start dragging
//                ndk_helper::Vec2 v;
//                eng->drag_detector_.GetPointer(v);
//                eng->TransformPosition(v);
//                eng->tap_camera_.BeginDrag(v);
//            } else if (dragState & ndk_helper::GESTURE_STATE_MOVE) {
//                ndk_helper::Vec2 v;
//                eng->drag_detector_.GetPointer(v);
//                eng->TransformPosition(v);
//                eng->tap_camera_.Drag(v);
//            } else if (dragState & ndk_helper::GESTURE_STATE_END) {
//                eng->tap_camera_.EndDrag();
//            }
//
//            // Handle pinch state
//            if (pinchState & ndk_helper::GESTURE_STATE_START) {
//                // Start new pinch
//                ndk_helper::Vec2 v1;
//                ndk_helper::Vec2 v2;
//                eng->pinch_detector_.GetPointers(v1, v2);
//                eng->TransformPosition(v1);
//                eng->TransformPosition(v2);
//                eng->tap_camera_.BeginPinch(v1, v2);
//            } else if (pinchState & ndk_helper::GESTURE_STATE_MOVE) {
//                // Multi touch
//                // Start new pinch
//                ndk_helper::Vec2 v1;
//                ndk_helper::Vec2 v2;
//                eng->pinch_detector_.GetPointers(v1, v2);
//                eng->TransformPosition(v1);
//                eng->TransformPosition(v2);
//                eng->tap_camera_.Pinch(v1, v2);
//            }
//        }
//        return 1;
//    }
    return 0;
}

int FlappyEngine::InitDisplay() {
    return 0;
}

void FlappyEngine::DrawFrame() {

}

void FlappyEngine::TermDisplay() {

}

void FlappyEngine::TrimMemory() {

}

bool FlappyEngine::onActivate() {
    mPtrGLContext->Resume(mPtrAndroidApp->window);
    mPtrSensorManager->Resume();
    return true;
}

void FlappyEngine::onDeactivate() {
    LogWrapper::info("FlappyEngine::onDeactivate()");
    mPtrGLContext->Suspend();
    mPtrSensorManager->Suspend();
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
    LogWrapper::info("FlappyEngine::onDestroyWindow()");
    mPtrGLContext->Invalidate();
}

void FlappyEngine::onGainFocus() {

}

void FlappyEngine::onLostFocus() {

}




