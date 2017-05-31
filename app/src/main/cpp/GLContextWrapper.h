#pragma once

#include <EGL/egl.h>
#include <GLES3/gl3.h>

class GLContextWrapper {
private:
    // EGL configurations
    ANativeWindow* window_;
    EGLDisplay display_;
    EGLSurface surface_;
    EGLContext context_;
    EGLConfig config_;

    // Screen parameters
    int32_t screen_width_;
    int32_t screen_height_;
    int32_t color_size_;
    int32_t depth_size_;

    // Flags
//    bool gles_initialized_;
    bool egl_context_initialized_;
//    bool es3_supported_;
//    float gl_version_;
    bool context_valid_;

//    void InitGLES();
    void DestroyContext();
    bool InitEGLSurface();
    bool InitEGLContext();

    GLContextWrapper(GLContextWrapper const&);
    void operator=(GLContextWrapper const&);

public:

    GLContextWrapper();
    ~GLContextWrapper();

    bool Init(ANativeWindow* window);
    EGLint Swap();
    bool Invalidate();

    void Suspend();
    EGLint Resume(ANativeWindow* window);

    int32_t GetScreenWidth() const { return screen_width_; }
    int32_t GetScreenHeight() const { return screen_height_; }

    int32_t GetBufferColorSize() const { return color_size_; }
    int32_t GetBufferDepthSize() const { return depth_size_; }
//    float GetGLVersion() const { return gl_version_; }
    bool CheckExtension(const char* extension);

    EGLDisplay GetDisplay() const { return display_; }
    EGLSurface GetSurface() const { return surface_; }
};

