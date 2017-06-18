#pragma once

#include <vector>

#include <GLES3/gl3.h>

class Texture
{
public:
    Texture();
    ~Texture();

    Texture(Texture const &) = default;
    Texture & operator=(Texture const &) = default;

    void SetInternalFormat(GLuint internalFormat) noexcept;
    void SetImageFormat(GLuint imageFormat) noexcept;

    GLuint GetInternalFormat() const noexcept;
    GLuint GetImageFormat() const noexcept;
    GLuint GetId() const noexcept;
    float  GetWidth() const noexcept;
    float  GetHeight() const noexcept;

    void Generate(GLint width,
                  GLint height,
                  std::vector<uint8_t> const & textureRaw);
    void Bind() const noexcept ;

private:
    GLuint mId;
    GLuint mInternalFormat;
    GLuint mImageFormat;
    GLuint mWrapS;
    GLuint mWrapT;
    GLuint mFilterMin;
    GLuint mFilterMax;
    float mWidth;
    float mHeight;
    float mAspectRatio;
};
