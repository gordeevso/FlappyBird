#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "Shader.h"
#include "Texture.h"
#include "Ui.h"
#include "GameTypes.h"
#include "Utilities.h"

using ShaderMap = std::unordered_map<std::string, Shader>;
using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture>>;
using UiStringMap = EnumKeyUnorderedMap<GameState, std::vector<UiString>>;

class ResourceManager {
public:
    static void LoadShader(std::string const &vsFilePath,
                           std::string const &fsFilePath,
                           std::string const &programName);
    static void LoadTexture(std::string const &textureFilePath,
                            GLboolean alpha,
                            std::string const &name);
    static void LoadUiStrings(std::string const & uiFilePath);
    static void FreeTextures();
    static void FreeShaders();
    static void Free();

    static Shader & GetShader(std::string const &name);
    static std::shared_ptr<Texture> GetTexture(std::string const &name);
    static std::vector<std::string> const & GetTextureNames();
    static std::vector<UiString> const  & GetUiStrings(GameState gameState);

    static void Read(std::string path, std::vector<uint8_t> & pBuffer, size_t sizeBytes = 0);
private:
    ResourceManager() = delete;
    ResourceManager(ResourceManager const &) = delete;
    ResourceManager &operator=(ResourceManager const &) = delete;

private:
    static ShaderMap mShaders;
    static TextureMap mTextures;
    static UiStringMap mUiStrings;
    static std::vector<std::string> mTextureNames;
};


