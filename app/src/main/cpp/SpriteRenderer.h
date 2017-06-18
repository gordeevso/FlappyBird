#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"

class SpriteRenderer
{
public:
    SpriteRenderer();
    SpriteRenderer(SpriteRenderer const &) = delete;
    SpriteRenderer & operator=(SpriteRenderer const &) = delete;
    ~SpriteRenderer();

    void DrawSprite(std::shared_ptr<Texture> texture,
                    glm::vec2 const & position,
                    glm::vec2 const & size,
                    GLfloat rotate_degrees,
                    glm::vec3 const & color);

private:
    Shader mShader;
    GLuint mVAO;
    GLuint mVBO;

private:
    void InitSpriteRenderData();
};

