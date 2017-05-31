#pragma once

#include <memory>

#include <android_native_app_glue.h>

#include "GLContextWrapper.h"
#include "SensorManager.h"
#include "LogWrapper.h"

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
    static int32_t AndroidInputHandler(android_app *app, AInputEvent *event);


    void SetState(android_app* state);
    int InitDisplay();
    void LoadResources();
    void UnloadResources();
    void DrawFrame();
    void TermDisplay();
    void TrimMemory();
    bool IsReady();

    void UpdatePosition(AInputEvent* event, int32_t iIndex, float& fX, float& fY);

//    void InitSensors();
//    void ProcessSensors(int32_t id);
//    void SuspendSensors();
//    void ResumeSensors();
private:
//    TeapotRenderer renderer_;
    android_app* mPtrAndroidApp;

    std::shared_ptr<GLContextWrapper> mPtrGLContext;
    std::shared_ptr<SensorManager> mPtrSensorManager;

    bool mInitializedResource;
    bool mHasFocus;
    bool mFinishActivity;

//    ndk_helper::DoubletapDetector doubletap_detector_;
//    ndk_helper::PinchDetector pinch_detector_;
//    ndk_helper::DragDetector drag_detector_;
//    ndk_helper::PerfMonitor monitor_;






//    void UpdateFPS(float fFPS);
//    void ShowUI();
//    void TransformPosition(ndk_helper::Vec2& vec);


};