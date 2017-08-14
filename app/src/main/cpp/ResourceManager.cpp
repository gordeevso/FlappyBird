#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

#include <SOIL2.h>
#include <tinyxml2.h>

#include "ResourceManager.h"
#include "Log.h"
#include "GLState.h"
#include "Android.h"
#include "Utilities.h"

TextureMap ResourceManager::mTextures;
ShaderMap ResourceManager::mShaders;
UiStringMap ResourceManager::mUiStrings;
std::vector<std::string> ResourceManager::mTextureNames;

//Shader-specific functions
Shader & ResourceManager::GetShader(std::string const &name) {
    auto it = mShaders.find(name);
    assert(it != mShaders.end());
    return it->second;
}

void ResourceManager::LoadShader(std::string const &vsFilePath,
                                 std::string const &fsFilePath,
                                 std::string const &programName) {
    auto result = mShaders.find(programName);
    if(result == mShaders.end()) {

        std::vector<uint8_t> vsSourceRaw{};
        Read(vsFilePath, vsSourceRaw);

        std::vector<uint8_t> fsSourceRaw{};
        Read(fsFilePath, fsSourceRaw);

        mShaders.insert(std::make_pair(programName, Shader{}));
        mShaders[programName].CreateProgram(std::string{vsSourceRaw.begin(), vsSourceRaw.end()},
                                             std::string{fsSourceRaw.begin(), fsSourceRaw.end()});

        Log::debug("LOAD SHADER SUCCESS : %s", vsFilePath.c_str());
        Log::debug("LOAD SHADER SUCCESS : %s", fsFilePath.c_str());
    }
}



//Texture-specific functions
std::shared_ptr<Texture> ResourceManager::GetTexture(std::string const &name) {
    auto it = mTextures.find(name);
    assert(it != mTextures.end());
    return it->second;
}

void ResourceManager::LoadTexture(std::string const &textureFilePath,
                                  GLboolean alpha,
                                  std::string const &name) {
    mTextures[name] = std::make_shared<Texture>(Texture {});
    mTextureNames.push_back(name);

    if (alpha) {
        mTextures[name]->SetImageFormat(GL_RGBA);
        mTextures[name]->SetInternalFormat(GL_RGBA);
    }

    int32_t width{};
    int32_t height{};

    Log::info("LOADING TEXTURE %s", textureFilePath.c_str());

    std::vector<uint8_t> textureRaw {};
    Read(textureFilePath, textureRaw);

    SOIL_load_image_from_memory(textureRaw.data(),
                                textureRaw.size(),
                                &width,
                                &height,
                                nullptr,
                                alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

    mTextures[name]->Generate(width, height, textureRaw);

    Log::info("LOADED TEXTURE %s SUCCESS", textureFilePath.c_str());
}

std::vector<std::string> const & ResourceManager::GetTextureNames() {
    return mTextureNames;
}


void ResourceManager::FreeTextures() {
    mTextureNames.clear();
    mTextures.clear();
}

void ResourceManager::FreeShaders() {
    mShaders.clear();
}

void ResourceManager::Free() {
    FreeTextures();
    FreeShaders();
}


void ResourceManager::Read(std::string path,
                           std::vector<uint8_t> & pBuffer,
                           size_t sizeBytes) {
    AAssetManager* assetManager = Android::GetInstance().GetAndroidApp()->activity->assetManager;
    AAsset *asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);

    if (asset == nullptr) {
        Log::debug("Error reading file : asset = null");
        throw std::invalid_argument("Error opening file : " + path);
    }

    if(sizeBytes == 0) {
        sizeBytes = static_cast<size_t>(AAsset_getLength(asset));
    }

    pBuffer.resize(sizeBytes);
    int32_t readCount = AAsset_read(asset, pBuffer.data(), sizeBytes);

    AAsset_close(asset);

    if(readCount < 0) {
        Log::debug("Error reading file : ");
        throw std::runtime_error("Error reading file : " + path);
    }
    if(readCount != sizeBytes) {
        Log::debug("Error reading file not fully : ");
        throw std::runtime_error("Error reading file not fully : " + path);
    }

    Log::debug("READ SUCCESS %s", path.c_str());
}

void ResourceManager::LoadUiStrings(std::string const &uiFilePath) {
    tinyxml2::XMLDocument sceneXml;
    std::vector<uint8_t> xmlBuffer;
    ResourceManager::Read(uiFilePath, xmlBuffer);
    auto result = sceneXml.Parse(std::string(xmlBuffer.begin(), xmlBuffer.end()).c_str());

    Log::debug("LOAD Settings %s", uiFilePath.c_str());
    if (result != tinyxml2::XMLError::XML_SUCCESS) {
        assert(result == tinyxml2::XMLError::XML_SUCCESS);
    }

    std::vector<UiString> uiStrings;
    tinyxml2::XMLElement *uiXmlRoot = sceneXml.RootElement();
    for (tinyxml2::XMLElement *ptrNodeState = uiXmlRoot->FirstChildElement();
         ptrNodeState;
         ptrNodeState = ptrNodeState->NextSiblingElement()) {

        assert(ptrNodeState);
        uiStrings.clear();
        GameState curState {};

        for (tinyxml2::XMLElement *ptrNodeString = ptrNodeState->FirstChildElement();
             ptrNodeString;
             ptrNodeString = ptrNodeString->NextSiblingElement()) {

            assert(ptrNodeString);

            std::string strState = ptrNodeString->Attribute("Type");
            curState = StrToGameState(strState);

            std::string curText = ptrNodeString->Attribute("Text");
            std::string strLayout = ptrNodeString->Attribute("Layout");
            LayoutType  curLayout = StrToLayout(strLayout);

            bool curIsStatic = ptrNodeString->BoolAttribute("Static");

            int32_t curDistPixels = ptrNodeString->IntAttribute("DistCharsPixels");

            int32_t scaleX = ptrNodeString->IntAttribute("ScaleX");
            int32_t scaleY = ptrNodeString->IntAttribute("ScaleY");
            int32_t scaleMaxX = ptrNodeString->IntAttribute("ScaleMaxX");
            int32_t scaleMaxY = ptrNodeString->IntAttribute("ScaleMaxY");

            assert(scaleX <= scaleMaxX && scaleX >= 0 && scaleMaxX >= 0);
            assert(scaleY <= scaleMaxY && scaleY >= 0 && scaleMaxY >= 0);

            float posXCenter = scaleX == 0 || scaleMaxX == 0 ? 0.f : static_cast<GLfloat>(GLState::GetInstance().GetScreenWidth()) / scaleMaxX * scaleX;
            float posYCenter = scaleY == 0 || scaleMaxX == 0 ? 0.f : static_cast<GLfloat>(GLState::GetInstance().GetScreenHeight()) / scaleMaxY * scaleY;

            float targetWidth = ptrNodeString->FloatAttribute("ScaleW") * GLState::GetInstance().GetScreenWidth();
            float targetHeight = ptrNodeString->FloatAttribute("ScaleH") * GLState::GetInstance().GetScreenHeight();

            glm::vec2 curTopLeft {posXCenter - targetWidth / 2, posYCenter - targetHeight / 2};
            glm::vec2 curBottomRight {posXCenter + targetWidth / 2, posYCenter + targetHeight / 2};

            tinyxml2::XMLElement *ptrNodeColor = ptrNodeString->FirstChildElement();
            assert(ptrNodeColor);

            float r = ptrNodeColor->FloatAttribute("r");
            float g = ptrNodeColor->FloatAttribute("g");
            float b = ptrNodeColor->FloatAttribute("b");
            Log::debug("LOAD Settings %s SUCCESS", uiFilePath.c_str());

            glm::vec3 curColor {r, g, b};


            uiStrings.push_back({curText,
                                curIsStatic,
                                curLayout,
                                curDistPixels,
                                curColor,
                                curTopLeft,
                                curBottomRight});

        }

        mUiStrings.insert(std::make_pair(curState, uiStrings));

    }
    Log::debug("LOAD Settings %s SUCCESS", uiFilePath.c_str());


}

std::vector<UiString> const & ResourceManager::GetUiStrings(GameState gameState) {
    auto resUiStringsIt = mUiStrings.find(gameState);
    if(resUiStringsIt != mUiStrings.end()) {
        return resUiStringsIt->second;
    }
    else {
        Log::error("RESOURCE MANAGER : no such data");
        assert(false);
    }
}

