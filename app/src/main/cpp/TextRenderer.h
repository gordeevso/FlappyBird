#pragma once

#include <unordered_map>
#include <vector>
#include <list>

#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Shader.h"
#include "GameTypes.h"


class TextRenderer;

struct FTString {
    std::string             text;
    glm::vec2               size;
    glm::vec2               topLeft;
    glm::vec3               color;
    std::vector<glm::vec2>  positions;
    GameState               state;
    void Draw(std::shared_ptr<TextRenderer> const & textRenderer) const;
};

class TextRenderer {
public:
    TextRenderer() = default;
    ~TextRenderer();
    TextRenderer(TextRenderer const &) = default;
    TextRenderer &operator=(TextRenderer const &) = default;

    void Init(std::string const & fontPath, size_t fontSize);
    void CalcUiString(UiString const &uiString, FTString & ftString);

    void Draw(FTString const &ftString);

private:
    std::unordered_map<uint8_t, FTCharacter> mCharactersMap;

    Shader mShader;
    GLuint mVAO;
    GLuint mVBO;
    FT_Face mPtrFTFace;
    FT_Library mPtrFTLib;
};


