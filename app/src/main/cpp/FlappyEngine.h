#pragma once

#include <memory>

#include <android_native_app_glue.h>

#include "GLContextWrapper.h"
#include "LogWrapper.h"
#include "TouchDetector.h"

class FlappyEngine
{

public:
    static FlappyEngine * GetInstance() {
        static FlappyEngine instance;
        return &instance;
    }

private:
    FlappyEngine();
    FlappyEngine(FlappyEngine const &);
    FlappyEngine & operator=(FlappyEngine const &);

public:

    ~FlappyEngine();
    void Run();
    void Init(android_app * app);
    void GetFocus();
    void GiveFocus();

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

    static void AndroidStateHandler(struct android_app *app, int32_t cmd);
    static int32_t AndroidTouchHandler(android_app *app, AInputEvent *event);

    void LoadResources();
    void UnloadResources();
    void DrawFrame();
    void TrimMemory();

private:
    android_app* mPtrAndroidApp;

    std::shared_ptr<GLContextWrapper> mPtrGLContext;

    bool mInitializedResource;
    bool mHasFocus;
    bool mFinishActivity;
};
