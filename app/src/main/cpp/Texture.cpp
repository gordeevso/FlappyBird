#include <exception>
#include <stdexcept>
#include <SOIL2.h>
#include <vector>

#include "Texture.h"

Texture::Texture()
        : mInternalFormat(GL_RGB),
          mImageFormat(GL_RGB),
          mWrapS(GL_REPEAT),
          mWrapT(GL_REPEAT),
          mFilterMin(GL_LINEAR),
          mFilterMax(GL_LINEAR)
{}

Texture::~Texture() {
    glDeleteTextures(1, &mId);
}

void Texture::Generate(GLint width,
                       GLint height,
                       std::vector<uint8_t> const & textureRaw)
{
    if(width <= 0 || height <= 0) {
        throw std::logic_error("Parameters must be positive");
    }

    mWidth = width;
    mHeight = height;
    mAspectRatio = static_cast<GLfloat>(width)/height;

    mId = SOIL_load_OGL_texture_from_memory(textureRaw.data(),
                                            textureRaw.size(),
                                            SOIL_LOAD_AUTO,
                                            SOIL_CREATE_NEW_ID,
                                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT);

}

void Texture::Bind() const noexcept {
    glBindTexture(GL_TEXTURE_2D, mId);
}

void Texture::SetInternalFormat(GLuint internalFormat) noexcept {
    mInternalFormat = internalFormat;
}

void Texture::SetImageFormat(GLuint imageFormat) noexcept {
    mImageFormat = imageFormat;
}

GLuint Texture::GetInternalFormat() const noexcept {
    return mInternalFormat;
}

GLuint Texture::GetImageFormat() const noexcept {
    return mImageFormat;
}

GLuint Texture::GetId() const noexcept {
    return mId;
}

float Texture::GetWidth() const noexcept {
    return mWidth;
}

float Texture::GetHeight() const noexcept {
    return mHeight;
}

float Texture::GetRatio() const noexcept {
    return mAspectRatio;
}
