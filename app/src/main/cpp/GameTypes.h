#pragma once

#include <vector>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

enum class GameState {
    START,
    ACTIVE,
    PAUSE,
    FINISH
};

struct FTCharacter {
    GLuint      mId;
    FT_UInt     index;
    FT_Glyph    glyph;
    FT_Vector   delta;
    int32_t     belowBaseline;
    int32_t     bearingY;
    glm::vec2   size;
};

struct FTString {
    std::string             text;
    glm::vec2               size;
    glm::vec2               topLeft;
    glm::vec3               color;
    std::vector<glm::vec2>  positions;
};

enum class LayoutType {
    CENTER,
    LEFT,
    RIGHT
};

struct UiString {
    std::string text;
    bool isStatic;
    LayoutType layout;
    int32_t charToCharDistPixels;
    glm::vec3 color;
    glm::vec2 topLeft;
    glm::vec2 bottomRight;
};

