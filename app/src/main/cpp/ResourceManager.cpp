#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

#include <SOIL2.h>

#include "ResourceManager.h"
#include "LogWrapper.h"
#include "GLContextWrapper.h"
#include "Android.h"

TextureMap ResourceManager::mTextures;
ShaderMap ResourceManager::mShaders;
std::vector<std::string> ResourceManager::mTextureNames;

//Shader-specific functions
Shader & ResourceManager::GetShader(std::string const &name) {
    return mShaders[name];
}

void ResourceManager::LoadShader(std::string const &vs_file_path,
                                 std::string const &fs_file_path,
                                 std::string const &program_name) {

    std::vector<uint8_t> vsSourceRaw {};
    Read(vs_file_path, vsSourceRaw);

    std::vector<uint8_t> fsSourceRaw {};
    Read(fs_file_path, fsSourceRaw);

    mShaders.insert(std::make_pair(program_name, Shader{}));
    mShaders[program_name].CreateProgram(std::string{vsSourceRaw.begin(), vsSourceRaw.end()},
                                         std::string{fsSourceRaw.begin(), fsSourceRaw.end()});

    LogWrapper::debug("LOAD SHADER SUCCESS : %s", vs_file_path.c_str());
    LogWrapper::debug("LOAD SHADER SUCCESS : %s", fs_file_path.c_str());

}



//Texture-specific functions
std::shared_ptr<Texture> ResourceManager::GetTexture(std::string const &name) {
    return mTextures[name];
}

void ResourceManager::LoadTexture(std::string const &texture_file,
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

    LogWrapper::info("LOADING TEXTURE %s", texture_file.c_str());

    std::vector<uint8_t> textureRaw {};
    Read(texture_file, textureRaw);

    SOIL_load_image_from_memory(textureRaw.data(),
                                textureRaw.size(),
                                &width,
                                &height,
                                nullptr,
                                alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);

    mTextures[name]->Generate(width, height, textureRaw);

    LogWrapper::info("LOADED TEXTURE %s SUCCESS", texture_file.c_str());
}

std::vector<std::string> const & ResourceManager::GetTextureNames() {
    return mTextureNames;
}

void ResourceManager::Read(std::string path,
                           std::vector<uint8_t> & pBuffer,
                           size_t sizeBytes) {
    AAssetManager* assetManager = Android::GetInstance().GetAndroidApp()->activity->assetManager;
    AAsset *asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_UNKNOWN);

    if (asset == nullptr) {
        LogWrapper::debug("Error reading file : asset = null");
        throw std::invalid_argument("Error opening file : " + path);
    }

    if(sizeBytes == 0) {
        sizeBytes = static_cast<size_t>(AAsset_getLength(asset));
    }

    pBuffer.resize(sizeBytes);
    int32_t readCount = AAsset_read(asset, pBuffer.data(), sizeBytes);

    AAsset_close(asset);

    if(readCount < 0) {
        LogWrapper::debug("Error reading file : ");
        throw std::runtime_error("Error reading file : " + path);
    }
    if(readCount != sizeBytes) {
        LogWrapper::debug("Error reading file not fully : ");
        throw std::runtime_error("Error reading file not fully : " + path);
    }

    LogWrapper::debug("READ SUCCESS %s", path.c_str());
}

