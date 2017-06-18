#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "Shader.h"
#include "Texture.h"

typedef std::unordered_map<std::string, Shader> ShaderMap;
typedef std::unordered_map<std::string, std::shared_ptr<Texture>> TextureMap;

class ResourceManager {
public:
    static void LoadShader(std::string const &vs_file_path,
                           std::string const &fs_file_path,
                           std::string const &program_name);
    static void LoadTexture(std::string const &texture_path,
                            GLboolean alpha,
                            std::string const &name);

    static Shader & GetShader(std::string const &name);
    static std::shared_ptr<Texture> GetTexture(std::string const &name);
    static std::vector<std::string> const & GetTextureNames();

private:
    ResourceManager() = delete;
    ResourceManager(ResourceManager const &) = delete;
    ResourceManager &operator=(ResourceManager const &) = delete;

    static void Read(std::string path, std::vector<uint8_t> & pBuffer, size_t sizeBytes = 0);

private:
    static ShaderMap mShaders;
    static TextureMap mTextures;
    static std::vector<std::string> mTextureNames;
};


