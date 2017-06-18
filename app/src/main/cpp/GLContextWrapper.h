#pragma once

#include <EGL/egl.h>
#include <GLES3/gl3.h>

class GLContextWrapper {
public:
    static GLContextWrapper & GetInstance() {
        static GLContextWrapper instance;
        return instance;
    }

private:
    GLContextWrapper();
    GLContextWrapper(GLContextWrapper const&) = delete;
    void operator=(GLContextWrapper const&) = delete;

public:
    ~GLContextWrapper();



    bool Init(ANativeWindow* window);
    EGLint Swap();
    bool Invalidate();

    void Suspend();
    EGLint Resume(ANativeWindow* window);

    bool IsInitialized() const { return mEGLContextInitialized; }

    int32_t GetScreenWidth() const { return mScreenWidth; }
    int32_t GetScreenHeight() const { return mScreenHeight; }

    int32_t GetBufferColorSize() const { return mColorSize; }
    int32_t GetBufferDepthSize() const { return mDepthSize; }

    bool CheckExtension(const char* extension);

    EGLDisplay GetDisplay() const { return mDisplay; }
    EGLSurface GetSurface() const { return mSurface; }

private:
    // EGL configurations
    ANativeWindow* mPtrWindow;
    EGLDisplay mDisplay;
    EGLSurface mSurface;
    EGLContext mContext;
    EGLConfig mConfig;

    // Screen parameters
    int32_t mScreenWidth;
    int32_t mScreenHeight;
    int32_t mColorSize;
    int32_t mDepthSize;

    // Flags
    bool mEGLContextInitialized;
    bool mIsContextValid;

    void DestroyContext();
    bool InitEGLSurface();
    bool InitEGLContext();

};

