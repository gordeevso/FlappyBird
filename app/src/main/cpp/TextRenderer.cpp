#include <glm/gtc/matrix_transform.hpp>

#include "GLState.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "Log.h"

std::string const TEXT_SHADER = "text_shader";

TextRenderer::TextRenderer(std::string const & fontPath,
                           size_t fontSize) : mCharactersMap{},
                                              mShader{},
                                              mVAO{},
                                              mVBO{}
{
    Init(fontPath, fontSize);
}

void TextRenderer::Init(std::string const & font, GLuint fontSize)
{
    ResourceManager::LoadShader("shaders/text.vs", "shaders/text.fs", TEXT_SHADER);
    mShader = ResourceManager::GetShader(TEXT_SHADER);
    mShader.Use();

    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<GLfloat>(GLState::GetInstance().GetScreenWidth()),
                                      static_cast<GLfloat>(GLState::GetInstance().GetScreenHeight()),
                                      0.0f);

    mShader.SetInteger("text", 0);
    mShader.SetMatrix4("projection", projection);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);

    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mCharactersMap.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        Log::error("ERROR::FREETYPE: Could not init FreeType Library");
        assert(false);
    }

    std::vector<uint8_t> fontBuffer;
    ResourceManager::Read(font, fontBuffer, 0);

    if (FT_New_Memory_Face(ft, fontBuffer.data(), fontBuffer.size(), 0, &mFT_Face)){
        Log::error("ERROR::FREETYPE: Failed to load font");
        assert(false);
    }

    FT_Set_Pixel_Sizes(mFT_Face, 0, fontSize);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (GLubyte c = 0; c < 128; c++)
    {
        if (FT_Load_Char(mFT_Face, c, FT_LOAD_RENDER)) {
            Log::error("ERROR::FREETYTPE: Failed to load Glyph %s ", c);
            assert(false);
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                mFT_Face->glyph->bitmap.width,
                mFT_Face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                mFT_Face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        FreeTypeCharacter character = {
                texture,
                glm::ivec2(mFT_Face->glyph->bitmap.width, mFT_Face->glyph->bitmap.rows),
                glm::ivec2(mFT_Face->glyph->bitmap_left, mFT_Face->glyph->bitmap_top),
                mFT_Face->glyph->advance.x
        };

        mCharactersMap.insert(std::make_pair(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(mFT_Face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text,
                              GLfloat x,
                              GLfloat y,
                              GLfloat scale,
                              glm::vec3 const & color)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShader.Use();
    mShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mVAO);

    for (auto c = text.begin(); c != text.end(); c++)
    {
        FreeTypeCharacter ch = mCharactersMap[*c];

        GLfloat posX = x + ch.mBearing.x * scale;
        GLfloat posY = y + (mCharactersMap['H'].mBearing.y - ch.mBearing.y) * scale;

        GLfloat w = ch.mSize.x * scale;
        GLfloat h = ch.mSize.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
                { posX,     posY + h,   0.f, 1.f },
                { posX + w, posY,       1.f, 0.f },
                { posX,     posY,       0.f, 0.f },

                { posX,     posY + h,   0.f, 1.f },
                { posX + w, posY + h,   1.f, 1.f },
                { posX + w, posY,       1.f, 0.f }
        };
        glBindTexture(GL_TEXTURE_2D, ch.mId);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Now advance cursors for next glyph
        x += (ch.mAdvance >> 6) * scale; // Bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);

}

TextRenderer::~TextRenderer() {
    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);

    for (auto c = mCharactersMap.begin(); c != mCharactersMap.end(); c++) {
        glDeleteTextures(1, &((*c).second.mId));
    }
}