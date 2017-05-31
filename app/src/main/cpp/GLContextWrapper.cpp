#include <string>

#include "GLContextWrapper.h"
#include "LogWrapper.h"

GLContextWrapper::GLContextWrapper()
        : display_(EGL_NO_DISPLAY),
          surface_(EGL_NO_SURFACE),
          context_(EGL_NO_CONTEXT),
          screen_width_(0),
          screen_height_(0),
//          gles_initialized_(false),
          egl_context_initialized_(false)
//          es3_supported_(false)
{}

//void GLContextWrapper::InitGLES() {
//    if (gles_initialized_) return;
//    //
//    // Initialize OpenGL ES 3 if available
//    //
//    const char* versionStr = (const char*)glGetString(GL_VERSION);
//    if (strstr(versionStr, "OpenGL ES 3.") && gl3stubInit()) {
//        es3_supported_ = true;
//        gl_version_ = 3.0f;
//    } else {
//        gl_version_ = 2.0f;
//    }
//
//    gles_initialized_ = true;
//}

GLContextWrapper::~GLContextWrapper() {
    DestroyContext();
}

bool GLContextWrapper::Init(ANativeWindow* window) {
    if (egl_context_initialized_) return true;

    //
    // Initialize EGL
    //
    window_ = window;
    InitEGLSurface();
    InitEGLContext();
//    InitGLES();

    egl_context_initialized_ = true;

    return true;
}

bool GLContextWrapper::InitEGLSurface() {
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display_, 0, 0);

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
    color_size_ = 8;
    depth_size_ = 24;


    EGLint num_configs;
    eglChooseConfig(display_, attribs, &config_, 1, &num_configs);

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

        eglChooseConfig(display_, attribs, &config_, 1, &num_configs);
        depth_size_ = 16;
    }

    if (!num_configs) {
        LogWrapper::warn("Unable to retrieve EGL config");
        return false;
    }

    surface_ = eglCreateWindowSurface(display_, config_, window_, NULL);
    eglQuerySurface(display_, surface_, EGL_WIDTH, &screen_width_);
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &screen_height_);

    return true;
}

bool GLContextWrapper::InitEGLContext() {
    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
                                      3,
                                      EGL_NONE};
    context_ = eglCreateContext(display_, config_, NULL, context_attribs);

    if (eglMakeCurrent(display_, surface_, surface_, context_) == EGL_FALSE) {
        LogWrapper::warn("Unable to eglMakeCurrent");
        return false;
    }

    context_valid_ = true;
    return true;
}

EGLint GLContextWrapper::Swap() {
    EGLBoolean swapBuffersRes = eglSwapBuffers(display_, surface_);
    if (!swapBuffersRes) {
        EGLint err = eglGetError();
        if (err == EGL_BAD_SURFACE) {
            // Recreate surface
            InitEGLSurface();
            return EGL_SUCCESS;  //consider glContext is valid
        } else if (err == EGL_CONTEXT_LOST || err == EGL_BAD_CONTEXT) {
            // Context has been lost!!
            context_valid_ = false;
            DestroyContext();
            InitEGLContext();
        }
        return err;
    }
    return EGL_SUCCESS;
}

void GLContextWrapper::DestroyContext() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) {
            eglDestroyContext(display_, context_);
        }

        if (surface_ != EGL_NO_SURFACE) {
            eglDestroySurface(display_, surface_);
        }
        eglTerminate(display_);
    }

    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
    surface_ = EGL_NO_SURFACE;
    context_valid_ = false;
}

EGLint GLContextWrapper::Resume(ANativeWindow* window) {
    if (egl_context_initialized_ == false) {
        Init(window);
        return EGL_SUCCESS;
    }
    int32_t original_widhth = screen_width_;
    int32_t original_height = screen_height_;

    // Create surface
    window_ = window;
    surface_ = eglCreateWindowSurface(display_, config_, window_, NULL);
    eglQuerySurface(display_, surface_, EGL_WIDTH, &screen_width_);
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &screen_height_);

    if (screen_width_ != original_widhth || screen_height_ != original_height) {
        // Screen resized
        LogWrapper::info("Screen resized");
    }

    if (eglMakeCurrent(display_, surface_, surface_, context_) == EGL_TRUE)
        return EGL_SUCCESS;

    EGLint err = eglGetError();
    LogWrapper::warn("Unable to eglMakeCurrent %d", err);

    if (err == EGL_CONTEXT_LOST) {
        // Recreate context
        LogWrapper::info("Re-creating egl context");
        InitEGLContext();
    } else {
        // Recreate surface
        DestroyContext();
        InitEGLSurface();
        InitEGLContext();
    }

    return err;
}

void GLContextWrapper::Suspend() {
    if (surface_ != EGL_NO_SURFACE) {
        eglDestroySurface(display_, surface_);
        surface_ = EGL_NO_SURFACE;
    }
}

bool GLContextWrapper::Invalidate() {
    DestroyContext();

    egl_context_initialized_ = false;
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
