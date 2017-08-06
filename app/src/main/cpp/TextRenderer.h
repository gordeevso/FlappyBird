#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"


struct FreeTypeCharacter {
    GLuint mId;          // GL texture id
    glm::ivec2 mSize;    // Size of glyph
    glm::ivec2 mBearing; // Offset from baseline to left/top of glyph
    FT_Pos mAdvance;     // Horizontal offset to advance to next glyph
};


class TextRenderer
{
public:
    TextRenderer(std::string const & fontPath, size_t fontSize);
    ~TextRenderer();
    TextRenderer(TextRenderer const &) = default;
    TextRenderer &operator=(TextRenderer const &) = default;

    void Init(std::string const & fontPath, size_t fontSize);
    FT_Face const & GetFTFace() const { return mFT_Face; }
    std::unordered_map<GLchar, FreeTypeCharacter> const & GetCharMap() const { return mCharactersMap; }

    void RenderText(std::string text,
                    GLfloat x,
                    GLfloat y,
                    GLfloat scale,
                    glm::vec3 const & color = glm::vec3(1.0f));

private:
    std::unordered_map<GLchar, FreeTypeCharacter> mCharactersMap;
    Shader mShader;
    GLuint mVAO;
    GLuint mVBO;
    FT_Face mFT_Face;
};
