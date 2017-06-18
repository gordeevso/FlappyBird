#include <string>
#include <cassert>

#include "GLContextWrapper.h"
#include "LogWrapper.h"

GLContextWrapper::GLContextWrapper()
        : mDisplay(EGL_NO_DISPLAY),
          mSurface(EGL_NO_SURFACE),
          mContext(EGL_NO_CONTEXT),
          mScreenWidth(0),
          mScreenHeight(0),
          mEGLContextInitialized(false)
{}

GLContextWrapper::~GLContextWrapper() {
    DestroyContext();
}

bool GLContextWrapper::Init(ANativeWindow* window) {
    if (mEGLContextInitialized)
        return true;

    mPtrWindow = window;
    InitEGLSurface();
    InitEGLContext();

    mEGLContextInitialized = true;

    return true;
}

bool GLContextWrapper::InitEGLSurface() {
    mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(mDisplay, 0, 0);

    const EGLint attribs[] = {EGL_RENDERABLE_TYPE,
                              EGL_OPENGL_ES2_BIT,
                              EGL_SURFACE_TYPE,
                              EGL_WINDOW_BIT,
                              EGL_BLUE_SIZE,
                              8,
                              EGL_GREEN_SIZE,
                              8,
                              EGL_RED_SIZE,
                              8,
                              EGL_DEPTH_SIZE,
                              24,
                              EGL_NONE};
    mColorSize = 8;
    mDepthSize = 24;


    EGLint num_configs;
    eglChooseConfig(mDisplay, attribs, &mConfig, 1, &num_configs);

    if (!num_configs) {
        // 16bit depth buffer
        const EGLint attribs[] = {EGL_RENDERABLE_TYPE,
                                  EGL_OPENGL_ES2_BIT,
                                  EGL_SURFACE_TYPE,
                                  EGL_WINDOW_BIT,
                                  EGL_BLUE_SIZE,
                                  8,
                                  EGL_GREEN_SIZE,
                                  8,
                                  EGL_RED_SIZE,
                                  8,
                                  EGL_DEPTH_SIZE,
                                  16,
                                  EGL_NONE};

        eglChooseConfig(mDisplay, attribs, &mConfig, 1, &num_configs);
        mDepthSize = 16;
    }

    EGLint errorCode = eglGetError();
    LogWrapper::info("GLCONTEXT INIT EGL SURFACE %x", errorCode);
    assert(errorCode == EGL_SUCCESS);


    mSurface = eglCreateWindowSurface(mDisplay, mConfig, mPtrWindow, NULL);
    eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &mScreenWidth);
    eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &mScreenHeight);

    errorCode = eglGetError();
    LogWrapper::info("GLCONTEXT INIT EGL CREATE SURFACE %x", errorCode);
    assert(errorCode == EGL_SUCCESS);

    return true;
}

bool GLContextWrapper::InitEGLContext() {
    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
                                      3,
                                      EGL_NONE};
    mContext = eglCreateContext(mDisplay, mConfig, NULL, context_attribs);

    EGLint errorCode = eglGetError();
    LogWrapper::info("GLCONTEXT INIT EGL CREATE CONTEXT %x", errorCode);
    assert(errorCode == EGL_SUCCESS);

    eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);

    errorCode = eglGetError();
    LogWrapper::info("GLCONTEXT INIT EGL MAKECURRENT %x", errorCode);
    assert(errorCode == EGL_SUCCESS);

    mIsContextValid = true;
    return true;
}

EGLint GLContextWrapper::Swap() {
    EGLBoolean swapBuffersRes = eglSwapBuffers(mDisplay, mSurface);

    EGLint errorCode = eglGetError();
    if(errorCode != EGL_SUCCESS) {
        LogWrapper::info("GLCONTEXT SWAP %x", errorCode);
        assert(errorCode == EGL_SUCCESS);
    }

    if (!swapBuffersRes) {
        EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            // Recreate surface
            InitEGLSurface();
            return EGL_SUCCESS;  //consider glContext is valid
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            // Context has been lost!!
            mIsContextValid = false;
            DestroyContext();
            InitEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

void GLContextWrapper::DestroyContext() {
    if (mDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(mDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (mContext != EGL_NO_CONTEXT) {
            eglDestroyContext(mDisplay, mContext);
        }

        if (mSurface != EGL_NO_SURFACE) {
            eglDestroySurface(mDisplay, mSurface);
        }
        eglTerminate(mDisplay);
    }

    EGLint errorCode = eglGetError();
    LogWrapper::info("GLCONTEXT DESTROY CONTEXT %x", errorCode);
    assert(errorCode == EGL_SUCCESS);

    mDisplay = EGL_NO_DISPLAY;
    mContext = EGL_NO_CONTEXT;
    mSurface = EGL_NO_SURFACE;
    mIsContextValid = false;
}

EGLint GLContextWrapper::Resume(ANativeWindow* window) {
    GLContextWrapper::Init(window);

    EGLint errorCode = eglGetError();
    LogWrapper::info("GLCONTEXT RESUME %x", errorCode);
    assert(errorCode == EGL_SUCCESS);

    return errorCode;
}

void GLContextWrapper::Suspend() {
    if (mSurface != EGL_NO_SURFACE) {
        eglDestroySurface(mDisplay, mSurface);

        EGLint errorCode = eglGetError();
        LogWrapper::info("GLCONTEXT SUSPEND %x", errorCode);
        assert(errorCode == EGL_SUCCESS);

        mSurface = EGL_NO_SURFACE;
    }
}

bool GLContextWrapper::Invalidate() {
    DestroyContext();

    mEGLContextInitialized = false;
    return true;
}

bool GLContextWrapper::CheckExtension(const char* extension) {
    if (extension == NULL) return false;

    std::string extensions = std::string((char*)glGetString(GL_EXTENSIONS));
    std::string str = std::string(extension);
    str.append(" ");

    size_t pos = 0;
    if (extensions.find(extension, pos) != std::string::npos) {
        return true;
    }

    return false;
}
