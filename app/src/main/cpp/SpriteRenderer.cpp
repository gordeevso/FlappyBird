
#include "SpriteRenderer.h"
#include "GLContextWrapper.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

std::string const SPRITE_SHADER = "sprite_shader";

SpriteRenderer::SpriteRenderer() : mShader{},
                                   mVAO{},
                                   mVBO{} {
    InitSpriteRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void SpriteRenderer::InitSpriteRenderData()
{
//    std::cout << "loading shader\n";
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.fs", SPRITE_SHADER);
    mShader = ResourceManager::GetShader(SPRITE_SHADER);

//    std::cout << "loading textures \n";
    ResourceManager::LoadTexture("textures/cat.png", GL_TRUE, "cat");


    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(GLContextWrapper::GetInstance().GetScreenWidth()),
                                      static_cast<GLfloat>(GLContextWrapper::GetInstance().GetScreenHeight()),
                                      0.0f,
                                      -1.0f,
                                      1.0f);

    mShader.Use();
    mShader.SetInteger("sprite", 0);
    mShader.SetMatrix4("projection", projection);

    GLfloat vertices[] =
            {
                    0.0f, 0.0f,     0.0f, 0.0f,
                    1.0f, 0.0f,     1.0f, 0.0f,
                    0.0f, 1.0f,     0.0f, 1.0f,
                    1.0f, 1.0f,     1.0f, 1.0f
            };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindVertexArray(mVAO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(std::shared_ptr<Texture> texture,
                                glm::vec2 const & position,
                                glm::vec2 const & size,
                                GLfloat rotate_degrees,
                                glm::vec3 const & color)
{
    mShader.Use();
    glm::mat4 model;

    model = glm::translate(model, glm::vec3(position, 0.0f));
    // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    // Move origin of rotation to center of quad
    model = glm::rotate(model, glm::radians(rotate_degrees), glm::vec3(0.0f, 0.0f, 1.0f));
    // Then rotate_degrees
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    // Move origin back
    model = glm::scale(model, glm::vec3(size, 1.0f));
    // Last scale

    mShader.SetMatrix4("model", model);
    mShader.SetVector3f("spriteColor", color);

    texture->Bind();
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}