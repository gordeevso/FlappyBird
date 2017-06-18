#pragma once

#include <memory>
#include <android_native_app_glue.h>

#include "TouchDetector.h"
#include "GLContextWrapper.h"

class Android {
public:

    static Android & GetInstance() {
        static Android instance;
        return instance;
    }

private:
    Android();
    Android(Android const &) = delete;
    Android &operator=(Android const &) = delete;

public:
    void Init(android_app * androidApp);
    void Run();

    android_app const * GetAndroidApp() const;

public:
    void HandleState(int32_t cmd);
    int32_t HandleInput(AInputEvent * inputEvent);
    static void AndroidStateHandler(android_app * androidApp, int32_t cmd);
    static int32_t AndroidTouchHandler(android_app * androidApp, AInputEvent * inputEvent);

private:
    void GetFocus();
    void GiveFocus();

private:

    android_app* mPtrAndroidApp;
    AAssetManager* mAssetManager;
    AAsset* mAsset;

    std::unique_ptr<TapDetector> mPtrTapDetector;

    bool mHasFocus;
    bool mFinishActivity;
};
