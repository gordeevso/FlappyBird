#include <glm/gtc/matrix_transform.hpp>
#include <freetype/ftrender.h>

#include "GLState.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "Log.h"


void TextRenderer::Init(std::string const & font, GLuint fontSize)
{
    mShader = ResourceManager::GetShader("text_shader");
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

    if (FT_Init_FreeType(&mPtrFTLib)) {
        Log::error("ERROR::FREETYPE: Could not init FreeType Library");
        assert(false);
    }

    std::vector<uint8_t> fontBuffer;
    ResourceManager::Read(font, fontBuffer, 0);

    if (FT_New_Memory_Face(mPtrFTLib, fontBuffer.data(), fontBuffer.size(), 0, &mPtrFTFace)){
        Log::error("ERROR::FREETYPE: Failed to load font");
        assert(false);
    }

    FT_Set_Pixel_Sizes(mPtrFTFace, 0, fontSize);

    FT_Bool       use_kerning {};
    FT_UInt       prevGlyphIndex {};
    FT_Error      error {};

    FTCharacter ftChar {};

    use_kerning = FT_HAS_KERNING( mPtrFTFace );
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (uint8_t c = 0; c < 128; c++) {
        /* convert character code to glyph index */
        ftChar.index = FT_Get_Char_Index( mPtrFTFace, c );

        /* retrieve kerning distance and move pen position */
        if ( use_kerning && prevGlyphIndex && ftChar.index ) {
            FT_Get_Kerning( mPtrFTFace,
                            prevGlyphIndex,
                            ftChar.index,
                            FT_KERNING_DEFAULT,
                            &ftChar.delta );
        }

        /* load glyph image into the slot without rendering */
        error = FT_Load_Glyph( mPtrFTFace, ftChar.index, FT_LOAD_DEFAULT );
        if(error){
            Log::error("ERROR::FREETYPE: Failed to load glyph");
            assert(false);
        }
        int32_t belowBaselineY = mPtrFTFace->glyph->metrics.height - mPtrFTFace->glyph->metrics.horiBearingY;
        ftChar.belowBaseline = belowBaselineY;
        ftChar.bearingY = mPtrFTFace->glyph->metrics.horiBearingY;
        /* extract glyph image and store it in our table */
        error = FT_Get_Glyph( mPtrFTFace->glyph, &ftChar.glyph );
        if(error){
            Log::error("ERROR::FREETYPE: Failed to get glyph");
            assert(false);
        }

        /* record current glyph index */
        prevGlyphIndex = ftChar.index;

        FT_Glyph   image;
        image = ftChar.glyph;

        error = FT_Glyph_To_Bitmap( &image,
                                    FT_RENDER_MODE_NORMAL,
                                    nullptr,
                                    false );
        if(error){
            Log::error("ERROR::FREETYPE: Failed to convert glyph to bitmap");
            assert(false);
        }

        FT_BitmapGlyph bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(image);

        glGenTextures(1, &ftChar.mId);
        glBindTexture(GL_TEXTURE_2D, ftChar.mId);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                bitmapGlyph->bitmap.width,
                bitmapGlyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                bitmapGlyph->bitmap.buffer
        );


        EGLint errorCode = eglGetError();
        if(errorCode != EGL_SUCCESS) {
            Log::info("ERROR::FREETYPE: Failed to load texture %x", errorCode);
            assert(errorCode == EGL_SUCCESS);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        ftChar.size.x = bitmapGlyph->bitmap.width;
        ftChar.size.y = bitmapGlyph->bitmap.rows;

        mCharactersMap.insert(std::make_pair(c, ftChar));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

}

void TextRenderer::Draw(FTString const &ftString) {
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mShader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mVAO);

    mShader.SetVector3f("textColor", ftString.color);

    size_t idx {};
    for( auto c : ftString.text) {
        auto ftChar = mCharactersMap[c];

        GLfloat posX = ftString.topLeft.x + ftString.positions[idx].x;
        GLfloat posY = ftString.topLeft.y + ftString.positions[idx].y;

        GLfloat w = ftChar.size.x;
        GLfloat h = ftChar.size.y;

        GLfloat vertices[6][4] = {
                { posX,     posY + h,   0.f, 1.f },
                { posX + w, posY,       1.f, 0.f },
                { posX,     posY,       0.f, 0.f },

                { posX,     posY + h,   0.f, 1.f },
                { posX + w, posY + h,   1.f, 1.f },
                { posX + w, posY,       1.f, 0.f }
        };
        glBindTexture(GL_TEXTURE_2D, ftChar.mId);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ++idx;
    }


    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_CULL_FACE);

}

TextRenderer::~TextRenderer() {
    FT_Done_Face(mPtrFTFace);
    FT_Done_FreeType(mPtrFTLib);

    glDeleteBuffers(1, &mVBO);
    glDeleteVertexArrays(1, &mVAO);

    for (auto c = mCharactersMap.begin(); c != mCharactersMap.end(); c++) {
        glDeleteTextures(1, &((*c).second.mId));
        FT_Done_Glyph(c->second.glyph);
    }
}

void TextRenderer::CalcUiString(UiString const &uiString, FTString & ftString) {
    assert(uiString.text.size() > 0);

    ftString.text = uiString.text;
    ftString.color = uiString.color;
    ftString.positions.reserve(uiString.text.size());

    int32_t posX {};
    int32_t posY {};

    posX = static_cast<int32_t>(uiString.topLeft.x);   /* start at topLeft param */
    posY = static_cast<int32_t>(uiString.topLeft.y);

    FT_BBox  bbox;
    FT_BBox  glyph_bbox;

    /* initialize string bbox to "empty" values */
    bbox.xMin = bbox.yMin =  std::numeric_limits<signed long>::max();
    bbox.xMax = bbox.yMax =  std::numeric_limits<signed long>::min();

    for (auto c : uiString.text) {
        auto itFTChar = mCharactersMap.find(c);
        if(itFTChar == mCharactersMap.end()) {
            Log::error("ERROR::FREETYPE: Failed to find FTChar in map");
            assert(false);
        }
        FTCharacter FTChar = itFTChar->second;

        posX += (FTChar.delta.x >> 6);

        /* store current pen position */
        if(FTChar.belowBaseline <= 0)
            posY = uiString.topLeft.y - FTChar.size.y;
        else
            posY = uiString.topLeft.y - (FTChar.bearingY >> 6);
//        if(FTChar.metrics.height - FTChar.metrics.horiBearingY > 0)
        ftString.positions.push_back({posX, posY});
        /* increment pen position */

        posX += uiString.charToCharDistPixels;
        posX += FTChar.size.x;
        /* for each glyph image, compute its bounding box, */
        /* translate it, and grow the string bbox          */
        FT_Glyph_Get_CBox( FTChar.glyph,
                           ft_glyph_bbox_pixels,
                           &glyph_bbox );

        glyph_bbox.xMin += ftString.positions.back().x;
        glyph_bbox.xMax += ftString.positions.back().x;
        glyph_bbox.yMin += ftString.positions.back().y;
        glyph_bbox.yMax += ftString.positions.back().y;

        if ( glyph_bbox.xMin < bbox.xMin )
            bbox.xMin = glyph_bbox.xMin;

        if ( glyph_bbox.yMin < bbox.yMin )
            bbox.yMin = glyph_bbox.yMin;

        if ( glyph_bbox.xMax > bbox.xMax )
            bbox.xMax = glyph_bbox.xMax;

        if ( glyph_bbox.yMax > bbox.yMax )
            bbox.yMax = glyph_bbox.yMax;
    }

    /* check that we really grew the string bbox */
    if ( bbox.xMin > bbox.xMax ) {
        bbox.xMin = 0;
        bbox.yMin = 0;
        bbox.xMax = 0;
        bbox.yMax = 0;
    }

    /* compute string dimensions in integer pixels */
    ftString.size.x = bbox.xMax - bbox.xMin;
    ftString.size.y = bbox.yMax - bbox.yMin;

    int32_t targetWidth = static_cast<int32_t>(uiString.bottomRight.x - uiString.topLeft.x);
    int32_t targetHeight = static_cast<int32_t>(uiString.bottomRight.y - uiString.topLeft.y);

    switch (uiString.layout) {
        case LayoutType::CENTER: {
            ftString.topLeft.x = (targetWidth - static_cast<int32_t>(ftString.size.x)) / 2;
            ftString.topLeft.y = (targetHeight - static_cast<int32_t>(ftString.size.y)) / 2;
            break;
        }
        case LayoutType::LEFT: {
            ftString.topLeft.x = uiString.topLeft.x;
            ftString.topLeft.y = (targetHeight - static_cast<int32_t>(ftString.size.y)) / 2;
            break;
        }
        case LayoutType::RIGHT: {
            ftString.topLeft.x = uiString.topLeft.x + (targetWidth - static_cast<int32_t>(ftString.size.x));
            ftString.topLeft.y = (targetHeight - static_cast<int32_t>(ftString.size.y)) / 2;
            break;
        }
    }

}


void FTString::Draw(std::shared_ptr<TextRenderer> const &textRenderer) const {
    textRenderer->Draw(*this);
}
