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



class TextRenderer {
public:
    TextRenderer() = default;
    ~TextRenderer();
    TextRenderer(TextRenderer const &) = default;
    TextRenderer &operator=(TextRenderer const &) = default;

    void Init(std::string const & fontPath, size_t fontSize);
    void AddRenderString(UiString const & uiString);

    void DrawStrings();

private:
    std::unordered_map<uint8_t, FTCharacter> mCharactersMap;
    std::list<FTString> mStringsStaticList;
    std::unordered_map<std::string, FTString> mStringsDynamicMap;
    Shader mShader;
    GLuint mVAO;
    GLuint mVBO;
    FT_Face mPtrFTFace;
    FT_Library mPtrFTLib;
};
